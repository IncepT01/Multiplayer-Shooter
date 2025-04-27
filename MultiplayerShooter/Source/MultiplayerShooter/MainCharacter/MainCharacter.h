// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiplayerShooter/Types/TurningInPlace.h"
#include "Camera/CameraComponent.h"
#include "MultiplayerShooter/ActorComponents/BuffComponent.h"
#include "MultiplayerShooter/ActorComponents/CombatComponent.h"
#include "MainCharacter.generated.h"

UCLASS()
class MULTIPLAYERSHOOTER_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastFire();

	virtual void OnRep_ReplicatedMovement() override;

	void Elim();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Elim();

	UPROPERTY(Replicated)
	bool bDisableGameplay = false;

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;
	
	float MouseSensitivity = 1.f;

	void UpdateHUDHealth();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	void ChatButtonPressed();
	void SettingsMenuButtonPressed();
	
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void PollInit();
	
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class ABaseWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(ABaseWeapon* LastWeapon);

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;

	void HideCameraIfCharacterClose();
 
	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	void PlayFireMontage(bool bAiming);
	void PlayHitReactMontage();
	
	void PlayElimMontage();

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	bool bElimmed = false;

	FTimerHandle ElimTimer;
 
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
 
	void ElimTimerFinished();

	/**
	 * Player health
	 */
 
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
 
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UPROPERTY(EditAnywhere)
	float DamageMultiplier = 1.f;
 
	UFUNCTION()
	void OnRep_Health(float LastHealth);

	UPROPERTY()
	class AMyPlayerController* MyPlayerController;

	void RotateInPlace(float DeltaTime);

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<ABaseWeapon> StarterWeapon;

	void SetOverlappingWeapon(ABaseWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;

	FVector GetHitTarget() const;

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }

	void TurnInPlace(float DeltaTime);

	ABaseWeapon* GetEquippedWeapon();

	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE void SetMaxHealth(float Amount) { MaxHealth = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	FORCEINLINE void SetCombatComponent(UCombatComponent* CombatComp) { Combat = CombatComp; }
	
	FORCEINLINE float GetTimeSinceLastMovementReplication() const { return TimeSinceLastMovementReplication; }
	FORCEINLINE void SetTimeSinceLastMovementReplication(float value) { TimeSinceLastMovementReplication = value; }

	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }

	FORCEINLINE float GetDamageMultiplier() const { return DamageMultiplier; }
	FORCEINLINE void SetDamageMultiplier(float Amount) { DamageMultiplier = Amount; }

	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }

	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }

	UPROPERTY()
	class AMainPlayerState* MainPlayerState;

	/** 
	 * Hit boxes used for server-side rewind
	 */
 
	UPROPERTY(EditAnywhere)
	class UBoxComponent* head;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;
 
	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
	class UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere)
	class UChatComponent* Chat;
};


