// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupSpawnPoint.h"

#include "DamagePickup.h"
#include "HealthPickup.h"
#include "Pickup.h"
#include "SpeedPickup.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/GameMode/MainGameMode.h"

APickupSpawnPoint::APickupSpawnPoint()
 {
 	PrimaryActorTick.bCanEverTick = true;
 	bReplicates = true;
 }
 
void APickupSpawnPoint::BeginPlay()
 {
 	Super::BeginPlay();
 	StartSpawnPickupTimer((AActor*)nullptr);
 }
 
void APickupSpawnPoint::SpawnPickup()
 {
 	int32 NumPickupClasses = PickupClasses.Num();
 	if (NumPickupClasses > 0)
 	{
 		int32 Selection = FMath::RandRange(0, NumPickupClasses - 1);
 		SpawnedPickup = GetWorld()->SpawnActor<APickup>(PickupClasses[Selection], GetActorTransform());
 		AMainGameMode* MainGameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

 		//Set Health pickup
 		if (SpawnedPickup->IsA(AHealthPickup::StaticClass()) && MainGameMode)
 		{
 			AHealthPickup* HealthPickup = Cast<AHealthPickup>(SpawnedPickup);
 			HealthPickup->HealAmount = MainGameMode->BuffList["HEALTH"];
 			UE_LOG(LogTemp, Warning, TEXT("SPawning Health pickup with amount: %f"), HealthPickup->HealAmount);
 		}

 		//Set Speed pickup
 		if (SpawnedPickup->IsA(ASpeedPickup::StaticClass()) && MainGameMode)
 		{
 			ASpeedPickup* SpeedPickup = Cast<ASpeedPickup>(SpawnedPickup);
 			SpeedPickup->BaseSpeedBuff = MainGameMode->BuffList["SPEED"];
 			SpeedPickup->CrouchSpeedBuff = MainGameMode->BuffList["SPEED"];
 			UE_LOG(LogTemp, Warning, TEXT("SPawning Health pickup with amount: %f"), SpeedPickup->BaseSpeedBuff);
 		}

 		//Set Damage pickup
 		if (SpawnedPickup->IsA(ADamagePickup::StaticClass()) && MainGameMode)
 		{
 			ADamagePickup* DamagePickup = Cast<ADamagePickup>(SpawnedPickup);
 			DamagePickup->BaseDamageBuff = MainGameMode->BuffList["DAMAGE"];
 			UE_LOG(LogTemp, Warning, TEXT("SPawning Health pickup with amount: %f"), DamagePickup->BaseDamageBuff);
 		}
 
 		if (HasAuthority() && SpawnedPickup)
 		{
 			SpawnedPickup->OnDestroyed.AddDynamic(this, &APickupSpawnPoint::StartSpawnPickupTimer);
 		}
 	}
 }
 
void APickupSpawnPoint::SpawnPickupTimerFinished()
 {
 	if (HasAuthority())
 	{
 		SpawnPickup();
 	}
 }
 
void APickupSpawnPoint::StartSpawnPickupTimer(AActor* DestroyedActor)
 {
 	const float SpawnTime = FMath::FRandRange(SpawnPickupTimeMin, SpawnPickupTimeMax);
 	GetWorldTimerManager().SetTimer(
		 SpawnPickupTimer,
		 this,
		 &APickupSpawnPoint::SpawnPickupTimerFinished,
		 SpawnTime
	 );
 }
 
void APickupSpawnPoint::Tick(float DeltaTime)
 {
 	Super::Tick(DeltaTime);

 }

