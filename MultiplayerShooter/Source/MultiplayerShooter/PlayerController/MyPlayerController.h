// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
 #include "CoreMinimal.h"
 #include "GameFramework/PlayerController.h"
#include "Sound/SoundClass.h"
#include "MyPlayerController.generated.h"
 
 /**
  * 
  */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighPingDelegate, bool, bHighPing, APlayerController*, PlayerController);

 UCLASS()
class MULTIPLAYERSHOOTER_API AMyPlayerController : public APlayerController
 {
 	GENERATED_BODY()
 public:
 	void SetHUDHealth(float Health, float MaxHealth);
 	void SetHUDScore(float Score);
 	void SetHUDWeaponAmmo(int32 Ammo);
 	void SetHUDMatchCountdown(float CountdownTime);
 	void SetHUDAnnouncementCountdown(float CountdownTime);
 	void HandleMatchHasStarted();

 	virtual void Tick(float DeltaTime) override;
 	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

 	virtual float GetServerTime(); // Synced with server world clock
 	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible

 	void ToggleSettingsMenu();

 	void OnMatchStateSet(FName State);
 	void HandleCooldown();
 	
 	void SetVolume(float Volume);
    void SetMouseSensitivity(float Sensitivity);
    void SetVolumeWithMix(USoundClass* SoundClass, USoundMix* SoundMix, float Volume);

 	float SingleTripTime = 0.f;

 	FHighPingDelegate HighPingDelegate;

 	UPROPERTY()
 	class AMainHUD* MainHUD;

 	UPROPERTY(EditAnywhere)
 	USoundMix* MyMix;

 	UPROPERTY(EditAnywhere)
 	USoundClass* SFXClass;
 	
 	UPROPERTY(EditAnywhere)
 	USoundClass* BackgroundMusicClass;

 	UPROPERTY()
 	class USettingsSaveGame* SaveGameObject;

 	UPROPERTY(EditAnywhere)
 	class USoundCue* BackgroundMusic;

 	UPROPERTY()
 	UAudioComponent* MusicAudioComponent;
 	
 protected:
 	virtual void BeginPlay() override;
 	virtual void OnPossess(APawn* InPawn) override;

 	void SetHUDTime();
 	void PollInit();

 	/** 
	 * Sync time between client and server
	 */
 
 	// Requests the current server time, passing in the client's time when the request was sent
 	UFUNCTION(Server, Reliable)
 	void ServerRequestServerTime(float TimeOfClientRequest);
 
 	// Reports the current server time to the client in response to ServerRequestServerTime
 	UFUNCTION(Client, Reliable)
 	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);
 
 	float ClientServerDelta = 0.f; // difference between client and server time

 	//How often we want to resync the client timer with the server. (Recalculate the RTT)
 	UPROPERTY(EditAnywhere, Category = Time)
 	float TimeSyncFrequency = 5.f;
 
 	float TimeSyncRunningTime = 0.f;
 	void CheckTimeSync(float DeltaTime);

 	UFUNCTION(Server, Reliable)
 	void ServerCheckMatchState();
 
 	UFUNCTION(Client, Reliable)
 	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

 	void HighPingWarning();
 	void StopHighPingWarning();
    void CheckPing(float DeltaTime);
 	
 private:
 	UFUNCTION(Server, Reliable)
 	void ServerTryCheckMatchState();

 	bool SettingsMenuIsOpen = false;

 	float LevelStartingTime = 0.f;
 	float MatchTime = 0.f;
 	float WarmupTime = 0.f;
 	float CooldownTime = 0.f;
 	uint32 CountdownInt = 0;

 	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
 	FName MatchState;
 
 	UFUNCTION()
 	void OnRep_MatchState();
 
 	UPROPERTY()
 	class UCharacterOverlay* CharacterOverlay;
 	bool bInitializeCharacterOverlay = false;
 
 	float HUDHealth;
 	float HUDMaxHealth;
 	float HUDScore;
 	int32 HUDDefeats;
 
 	float HighPingRunningTime = 0.f;
 
 	UPROPERTY(EditAnywhere)
 	float CheckPingFrequency = 20.f;
 
 	UPROPERTY(EditAnywhere)
 	float HighPingThreshold = 50.f;

 	UFUNCTION(Server, Reliable)
 	void ServerReportPingStatus(bool bHighPing);
 };
