// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MultiplayerShooter/HUD/MainHUD.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

//Forward declair
class ABaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	friend class AMainCharacter;

	void FireButtonPressed(bool bPressed);

	void EquipWeapon(ABaseWeapon* WeaponToEquip);

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
 
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	void Fire();
	void LocalFire(const FVector_NetQuantize& fnHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	virtual void Multicast_StartFiring(const FVector& fnHitTarget);

	UFUNCTION(Server, Reliable)
	void Server_Fire(bool bLocalFireButtonPressed, const FVector_NetQuantize& TracerTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

private:
	UPROPERTY()
	class AMainCharacter* Character;

	UPROPERTY()
	class AMyPlayerController* Controller;

	UPROPERTY()
	class AMainHUD* HUD;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	ABaseWeapon* EquippedWeapon;

	UPROPERTY(Replicated, ReplicatedUsing=OnRep_Aiming)
	bool bAiming = false;

	bool bAimButtonPressed = false;

	UFUNCTION()
	void OnRep_Aiming();

	bool bFireButtonPressed;

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	
	FVector HitTarget;

	FHUDPackage HUDPackage;
	//FOV settings
	float DefaultFOV;
 
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;
 
	float CurrentFOV;
 
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;
 
	void InterpFOV(float DeltaTime);
	
	 //Automatic fire 
 
	FTimerHandle FireTimer;
	bool bCanFire = true;
 
	void StartFireTimer();
	void FireTimerFinished();
	
	bool CanFire();
	
};
