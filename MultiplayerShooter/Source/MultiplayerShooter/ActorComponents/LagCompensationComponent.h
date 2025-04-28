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

	UPROPERTY()
	AMainCharacter* Character;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()
 
	UPROPERTY()
	bool bHitConfirmed;
 
	UPROPERTY()
	bool bHeadShot;
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

	/**
	 * HitScan
	 */
	FServerSideRewindResult ServerSideRewind(
		 class AMainCharacter* HitCharacter, 
		 const FVector_NetQuantize& TraceStart, 
		 const FVector_NetQuantize& HitLocation, 
		 float HitTime);

	/** 
	 * Projectile
	 */
	FServerSideRewindResult ProjectileServerSideRewind(
		AMainCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		AMainCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime,
		class ABaseWeapon* DamageCauser
	);
	

	UFUNCTION(Server, Reliable)
	void ProjectileServerScoreRequest(
		AMainCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);

	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);
	
	void CacheBoxPositions(AMainCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(AMainCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(AMainCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(AMainCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);
	void SaveFramePackage();

	FFramePackage GetFrameToCheck(AMainCharacter* HitCharacter, float HitTime);

	/** 
	 * Hitscan
	 */
	FServerSideRewindResult ConfirmHit(
		const FFramePackage& Package,
		AMainCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation);
 
	/** 
	* Projectile
	*/
	FServerSideRewindResult ProjectileConfirmHit(
		const FFramePackage& Package,
		AMainCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);
	

private:
	UPROPERTY()
	AMainCharacter* Character;
 
	UPROPERTY()
	class AMyPlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;
 
	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 3.f;
	
public:	

		
};
