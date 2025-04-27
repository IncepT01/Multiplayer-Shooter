// Fill out your copyright notice in the Description page of Project Settings.


#include "LagCompensationComponent.h"
#include "Components/BoxComponent.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/BaseWeapon/BaseWeapon.h"
#include "MultiplayerShooter/MultiplayerShooter.h"

// Sets default values for this component's properties
ULagCompensationComponent::ULagCompensationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SaveFramePackage();
}

void ULagCompensationComponent::SaveFramePackage()
{
	if (Character == nullptr || !Character->HasAuthority()) return;
	if (FrameHistory.Num() <= 1)
	{
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
	else
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
		
		//ShowFramePackage(ThisFrame, FColor::Red);
	}
}


void ULagCompensationComponent::SaveFramePackage(FFramePackage& Package)
{
	Character = Character == nullptr ? Cast<AMainCharacter>(GetOwner()) : Character;
	if (Character)
	{
		Package.Time = GetWorld()->GetTimeSeconds();
		for (auto& BoxPair : Character->HitCollisionBoxes)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
			Package.HitBoxInfo.Add(BoxPair.Key, BoxInformation);
		}
	}
}

void ULagCompensationComponent::ServerScoreRequest_Implementation(AMainCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, ABaseWeapon* DamageCauser)
{
	FServerSideRewindResult Confirm = ServerSideRewind(HitCharacter, TraceStart, HitLocation, HitTime);
 
	if (Character && HitCharacter && DamageCauser && Confirm.bHitConfirmed)
	{
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			DamageCauser->GetDamage(),
			Character->Controller,
			DamageCauser,
			UDamageType::StaticClass()
		);
	}
}
 
void ULagCompensationComponent::ShowFramePackage(const FFramePackage& Package, const FColor& Color)
{
	for (auto& BoxInfo : Package.HitBoxInfo)
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.Location,
			BoxInfo.Value.BoxExtent,
			FQuat(BoxInfo.Value.Rotation),
			Color,
			false,
			4.f
		);
	}
}


FServerSideRewindResult ULagCompensationComponent::ServerSideRewind(AMainCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
 {
 	bool bReturn =
 		HitCharacter == nullptr ||
 		HitCharacter->GetLagCompensation() == nullptr ||
 		HitCharacter->GetLagCompensation()->FrameHistory.GetHead() == nullptr ||
 		HitCharacter->GetLagCompensation()->FrameHistory.GetTail() == nullptr;
 	if (bReturn) return FServerSideRewindResult();
 	// Frame package that we check to verify a hit
 	FFramePackage FrameToCheck;
 	bool bShouldInterpolate = true;
 	// Frame history of the HitCharacter
 	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensation()->FrameHistory;
 	const float OldestHistoryTime = History.GetTail()->GetValue().Time;
 	const float NewestHistoryTime = History.GetHead()->GetValue().Time;
 	if (OldestHistoryTime > HitTime)
 	{
 		// too far back - too laggy to do SSR
 		return FServerSideRewindResult();
 	}
 	if (OldestHistoryTime == HitTime)
 	{
 		FrameToCheck = History.GetTail()->GetValue();
 		bShouldInterpolate = false;
 	}
 	if (NewestHistoryTime <= HitTime)
 	{
 		FrameToCheck = History.GetHead()->GetValue();
 		bShouldInterpolate = false;
 	}
 
 	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
 	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;
 	while (Older->GetValue().Time > HitTime) // is Older still younger than HitTime?
 	{
 		// March back until: OlderTime < HitTime < YoungerTime
 		if (Older->GetNextNode() == nullptr) break;
 		Older = Older->GetNextNode();
 		if (Older->GetValue().Time > HitTime)
 		{
 			Younger = Older;
 		}
 	}
 	if (Older->GetValue().Time == HitTime) // highly unlikely, but we found our frame to check
 	{
 		FrameToCheck = Older->GetValue();
 		bShouldInterpolate = false;
 	}
 	if (bShouldInterpolate)
 	{
 		// Interpolate between Younger and Older
 		FrameToCheck = InterpBetweenFrames(Older->GetValue(), Younger->GetValue(), HitTime);
 	}
	
 	return ConfirmHit(FrameToCheck, HitCharacter, TraceStart, HitLocation);
 }


FFramePackage ULagCompensationComponent::InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime)
{
	const float Distance = YoungerFrame.Time - OlderFrame.Time;
	const float InterpFraction = FMath::Clamp((HitTime - OlderFrame.Time) / Distance, 0.f, 1.f);
 
	FFramePackage InterpFramePackage;
	InterpFramePackage.Time = HitTime;
 
	for (auto& YoungerPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxInfoName = YoungerPair.Key;
 
		const FBoxInformation& OlderBox = OlderFrame.HitBoxInfo[BoxInfoName];
		const FBoxInformation& YoungerBox = YoungerFrame.HitBoxInfo[BoxInfoName];
 
		FBoxInformation InterpBoxInfo;
 
		InterpBoxInfo.Location = FMath::VInterpTo(OlderBox.Location, YoungerBox.Location, 1.f, InterpFraction);
		InterpBoxInfo.Rotation = FMath::RInterpTo(OlderBox.Rotation, YoungerBox.Rotation, 1.f, InterpFraction);
		InterpBoxInfo.BoxExtent = YoungerBox.BoxExtent;
 
		InterpFramePackage.HitBoxInfo.Add(BoxInfoName, InterpBoxInfo);
	}
 
	return InterpFramePackage;
}


FServerSideRewindResult ULagCompensationComponent::ConfirmHit(const FFramePackage& Package, AMainCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation)
 {
 	if (HitCharacter == nullptr) return FServerSideRewindResult();
 
 	FFramePackage CurrentFrame;
 	CacheBoxPositions(HitCharacter, CurrentFrame);
 	MoveBoxes(HitCharacter, Package);
 	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);
 
 	// Enable collision for the head first
 	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("head")];
 	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
 	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
 
 	FHitResult ConfirmHitResult;
 	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
 	UWorld* World = GetWorld();
 	if (World)
 	{
 		World->LineTraceSingleByChannel(
 			ConfirmHitResult,
 			TraceStart,
 			TraceEnd,
 			ECC_HitBox
 		);
 		if (ConfirmHitResult.bBlockingHit) // we hit the head, return early
 		{

 			if (ConfirmHitResult.Component.IsValid())
 			{
 				UBoxComponent* Box = Cast<UBoxComponent>(ConfirmHitResult.Component);
 				if (Box)
 				{
 					//DrawDebugBox(GetWorld(), Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()), FColor::Red, false, 8.f);
 				}
 			}
 			
 			ResetHitBoxes(HitCharacter, CurrentFrame);
 			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
 			return FServerSideRewindResult{ true, true };
 		}
 		else // didn't hit head, check the rest of the boxes
 		{
 			for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
 			{
 				if (HitBoxPair.Value != nullptr)
 				{
 					HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
 					HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
 				}
 			}
 			World->LineTraceSingleByChannel(
 				ConfirmHitResult,
 				TraceStart,
 				TraceEnd,
 				ECC_HitBox
 			);
 			if (ConfirmHitResult.bBlockingHit)
 			{
				if (ConfirmHitResult.Component.IsValid())
                 	{
                 		UBoxComponent* Box = Cast<UBoxComponent>(ConfirmHitResult.Component);
                 		if (Box)
                 		{
                 			//DrawDebugBox(GetWorld(), Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()), FColor::Blue, false, 8.f);
                 		}
                 	}
 				
 				ResetHitBoxes(HitCharacter, CurrentFrame);
 				EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);

 				GEngine->AddOnScreenDebugMessage(
							 -1,
							 15.f,
							 FColor::Blue,
							 FString(TEXT("Scored a hit with Lag compensation!"))
						 );
 				
 				return FServerSideRewindResult{ true, false };
 			}
 		}
 	}
 
 	ResetHitBoxes(HitCharacter, CurrentFrame);
 	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
 	return FServerSideRewindResult{ false, false };
 }
 
 void ULagCompensationComponent::CacheBoxPositions(AMainCharacter* HitCharacter, FFramePackage& OutFramePackage)
 {
 	if (HitCharacter == nullptr) return;
 	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
 	{
 		if (HitBoxPair.Value != nullptr)
 		{
 			FBoxInformation BoxInfo;
 			BoxInfo.Location = HitBoxPair.Value->GetComponentLocation();
 			BoxInfo.Rotation = HitBoxPair.Value->GetComponentRotation();
 			BoxInfo.BoxExtent = HitBoxPair.Value->GetScaledBoxExtent();
 			OutFramePackage.HitBoxInfo.Add(HitBoxPair.Key, BoxInfo);
 		}
 	}
 }
 
 void ULagCompensationComponent::MoveBoxes(AMainCharacter* HitCharacter, const FFramePackage& Package)
 {
 	if (HitCharacter == nullptr) return;
 	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
 	{
 		if (const FBoxInformation* BoxInfo = Package.HitBoxInfo.Find(HitBoxPair.Key))
 		{
 			if (HitBoxPair.Value != nullptr)
 			{
 				//HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
 				//HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
 				//HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].BoxExtent);

 				HitBoxPair.Value->SetWorldLocation(BoxInfo->Location);
 				HitBoxPair.Value->SetWorldRotation(BoxInfo->Rotation);
 				HitBoxPair.Value->SetBoxExtent(BoxInfo->BoxExtent);
 			}
 		}
 	}
 }
 
 void ULagCompensationComponent::ResetHitBoxes(AMainCharacter* HitCharacter, const FFramePackage& Package)
 {
 	if (HitCharacter == nullptr) return;
 	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
 	{
 		if (HitBoxPair.Value != nullptr)
 		{
 			HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
 			HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
 			HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].BoxExtent);
 			HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 		}
 	}
 }
 
 void ULagCompensationComponent::EnableCharacterMeshCollision(AMainCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled)
 {
 	if (HitCharacter && HitCharacter->GetMesh())
 	{
 		HitCharacter->GetMesh()->SetCollisionEnabled(CollisionEnabled);
 	}
 }

FServerSideRewindResult ULagCompensationComponent::ProjectileConfirmHit(const FFramePackage& Package, AMainCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
 {
 	FFramePackage CurrentFrame;
 	CacheBoxPositions(HitCharacter, CurrentFrame);
 	MoveBoxes(HitCharacter, Package);
 	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);
 
 	// Enable collision for the head first
 	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("head")];
 	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
 	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
 
 	FPredictProjectilePathParams PathParams;
 	PathParams.bTraceWithCollision = true;
 	PathParams.MaxSimTime = MaxRecordTime;
 	PathParams.LaunchVelocity = InitialVelocity;
 	PathParams.StartLocation = TraceStart;
 	PathParams.SimFrequency = 15.f;
 	PathParams.ProjectileRadius = 5.f;
 	PathParams.TraceChannel = ECC_HitBox;
 	PathParams.ActorsToIgnore.Add(GetOwner());
 	//PathParams.DrawDebugTime = 5.f;
 	//PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
 
 	FPredictProjectilePathResult PathResult;
 	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
 
 	if (PathResult.HitResult.bBlockingHit) // we hit the head, return early
 	{
 		if (PathResult.HitResult.Component.IsValid())
 		{
 			UBoxComponent* Box = Cast<UBoxComponent>(PathResult.HitResult.Component);
 			if (Box)
 			{
 				//DrawDebugBox(GetWorld(), Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()), FColor::Red, false, 8.f);
 			}
 		}
 
 		ResetHitBoxes(HitCharacter, CurrentFrame);
 		EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
 		return FServerSideRewindResult{ true, true };
 	}
 	else // we didn't hit the head; check the rest of the boxes
 	{
 		for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
 		{
 			if (HitBoxPair.Value != nullptr)
 			{
 				HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
 				HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
 			}
 		}
 
 		UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
 		if (PathResult.HitResult.bBlockingHit)
 		{
 			if (PathResult.HitResult.Component.IsValid())
 			{
 				UBoxComponent* Box = Cast<UBoxComponent>(PathResult.HitResult.Component);
 				if (Box)
 				{
 					//DrawDebugBox(GetWorld(), Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()), FColor::Blue, false, 8.f);
 				}
 			}

 			//GEngine->AddOnScreenDebugMessage(-1,15.f,FColor::Blue,FString(TEXT("Confirmed Projectile hit with SSR!")));
 
 			ResetHitBoxes(HitCharacter, CurrentFrame);
 			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
 			return FServerSideRewindResult{ true, false };
 		}
 	}
 
 	ResetHitBoxes(HitCharacter, CurrentFrame);
 	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
 	return FServerSideRewindResult{ false, false };
 }

FServerSideRewindResult ULagCompensationComponent::ProjectileServerSideRewind(AMainCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);
	return ProjectileConfirmHit(FrameToCheck, HitCharacter, TraceStart, InitialVelocity, HitTime);
}

FFramePackage ULagCompensationComponent::GetFrameToCheck(AMainCharacter* HitCharacter, float HitTime)
 {
 	bool bReturn =
 		HitCharacter == nullptr ||
 		HitCharacter->GetLagCompensation() == nullptr ||
 		HitCharacter->GetLagCompensation()->FrameHistory.GetHead() == nullptr ||
 		HitCharacter->GetLagCompensation()->FrameHistory.GetTail() == nullptr;
 	if (bReturn) return FFramePackage();
 	// Frame package that we check to verify a hit
 	FFramePackage FrameToCheck;
 	bool bShouldInterpolate = true;
 	// Frame history of the HitCharacter
 	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensation()->FrameHistory;
 	const float OldestHistoryTime = History.GetTail()->GetValue().Time;
 	const float NewestHistoryTime = History.GetHead()->GetValue().Time;
 	if (OldestHistoryTime > HitTime)
 	{
 		// too far back - too laggy to do SSR
 		return FFramePackage();
 	}
 	if (OldestHistoryTime == HitTime)
 	{
 		FrameToCheck = History.GetTail()->GetValue();
 		bShouldInterpolate = false;
 	}
 	if (NewestHistoryTime <= HitTime)
 	{
 		FrameToCheck = History.GetHead()->GetValue();
 		bShouldInterpolate = false;
 	}
 
 	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
 	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;
 	while (Older->GetValue().Time > HitTime) // is Older still younger than HitTime?
 	{
 		// March back until: OlderTime < HitTime < YoungerTime
 		if (Older->GetNextNode() == nullptr) break;
 		Older = Older->GetNextNode();
 		if (Older->GetValue().Time > HitTime)
 		{
 			Younger = Older;
 		}
 	}
 	if (Older->GetValue().Time == HitTime) // highly unlikely, but we found our frame to check
 	{
 		FrameToCheck = Older->GetValue();
 		bShouldInterpolate = false;
 	}
 	if (bShouldInterpolate)
 	{
 		// Interpolate between Younger and Older
 		FrameToCheck = InterpBetweenFrames(Older->GetValue(), Younger->GetValue(), HitTime);
 	}
 	FrameToCheck.Character = HitCharacter;
 	return FrameToCheck;
 }

void ULagCompensationComponent::ProjectileServerScoreRequest_Implementation(AMainCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
		FServerSideRewindResult Confirm = ProjectileServerSideRewind(HitCharacter, TraceStart, InitialVelocity, HitTime);
 
	if (Character && HitCharacter && Confirm.bHitConfirmed)
	{
		//UE_LOG(LogTemp, Warning, TEXT("SSR Damaging for: %f"), Character->GetEquippedWeapon()->GetDamage() * Character->GetDamageMultiplier());
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			Character->GetEquippedWeapon()->GetDamage() * Character->GetDamageMultiplier(),
			Character->Controller,
			Character->GetEquippedWeapon(),
			UDamageType::StaticClass()
		);
	}
}

