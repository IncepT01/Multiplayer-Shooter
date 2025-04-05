// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"


USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()
 
	UPROPERTY()
	FVector Location;
 
	UPROPERTY()
	FRotator Rotation;
 
	UPROPERTY()
	FVector BoxExtent;
};
 
USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()
 
	UPROPERTY()
	float Time;
 
	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULagCompensationComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	friend class AMainCharacter;

	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);

	void ServerSideRewind(
		 class AMainCharacter* HitCharacter, 
		 const FVector_NetQuantize& TraceStart, 
		 const FVector_NetQuantize& HitLocation, 
		 float HitTime);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);

	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);

private:
	UPROPERTY()
	AMainCharacter* Character;
 
	UPROPERTY()
	class AMyPlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;
 
	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;
	
public:	

		
};
