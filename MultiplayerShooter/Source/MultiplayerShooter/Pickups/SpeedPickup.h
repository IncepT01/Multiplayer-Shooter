// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "SpeedPickup.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ASpeedPickup : public APickup
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FString Name = "SPEED";
 
	UPROPERTY(EditAnywhere)
	float BaseSpeedBuff = 400.f;
 
	UPROPERTY(EditAnywhere)
	float CrouchSpeedBuff = 400.f;
 
	UPROPERTY(EditAnywhere)
	float SpeedBuffTime = 10.f;

	
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
