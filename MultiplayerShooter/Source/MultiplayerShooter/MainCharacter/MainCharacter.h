// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultiplayerShooter/Types/TurningInPlace.h"
#include "Camera/CameraComponent.h"
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
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();
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
 
	UFUNCTION()
	void OnRep_Health();
	
	class AMyPlayerController* MyPlayerController;

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
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	class AMainPlayerState* MainPlayerState;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;
	

};


