// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameMode.generated.h"

namespace MatchState
{
	extern MULTIPLAYERSHOOTER_API const FName Cooldown; // Match duration has been reached. Display winner and begin cooldown timer.
}

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AMainGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AMainCharacter* ElimmedCharacter, class AMyPlayerController* VictimController, AMyPlayerController*
	                              AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 5.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 20.f;
	
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 5.f;
 
	float LevelStartingTime = 0.f;

	TMap<FString, float> BuffList;

	UPROPERTY(EditAnywhere)
	USoundCue* BackgroundMusic;

	UPROPERTY()
	UAudioComponent* MusicAudioComponent;

	UAudioComponent* GetMusicAudioComponent() const;
 
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

	UFUNCTION()
	void HandleHighPing(bool bHighPing, APlayerController* PlayerController);

	virtual void PostLogin(APlayerController* NewPlayer) override;
 
private:
	float CountdownTime = 0.f;
};
