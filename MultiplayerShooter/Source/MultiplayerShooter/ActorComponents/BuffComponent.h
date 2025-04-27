// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class FBuffComponentSpeedBuffTest;
	
	UBuffComponent();
	friend class AMainCharacter;
	void Heal(float HealAmount, float HealingTime);

	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);

	void BuffDamage(float DamageMul, float BuffTime);

	void ResetSpeeds();
	void ResetDamage();

	virtual void BeginPlay() override;
protected:

	void HealRampUp(float DeltaTime);
 
private:
	UPROPERTY()
	class AMainCharacter* Character;

	/** 
	 * Heal buff
	 */

	bool bHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;

	/** 
	 * Speed buff
	 */
 
	FTimerHandle SpeedBuffTimer;
	FTimerHandle DamageBuffTimer;
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
 
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDamageBuff(float DamageMul);
 
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE AMainCharacter* GetCharacter() const { return Character; }
	FORCEINLINE bool bIsHealing() const { return bHealing; }
};
