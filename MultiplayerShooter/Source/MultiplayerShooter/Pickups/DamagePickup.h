// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "DamagePickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ADamagePickup : public APickup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString Name = "DAMAGE";
	
	UPROPERTY(EditAnywhere)
	float BaseDamageBuff = 2.f;
 
	UPROPERTY(EditAnywhere)
	float DamageBuffTime = 15.f;

protected:
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
