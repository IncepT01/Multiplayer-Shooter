// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
 #include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "MultiplayerShooter/ActorComponents/LagCompensationComponent.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "MultiplayerShooter/PlayerController/MyPlayerController.h"


AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = InitialSpeed;
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
 /*
	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithChannel = true;
	PathParams.bTraceWithCollision = true;
	PathParams.DrawDebugTime = 5.f;
	PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	PathParams.LaunchVelocity = GetActorForwardVector() * InitialSpeed;
	PathParams.MaxSimTime = 4.f;
	PathParams.ProjectileRadius = 5.f;
	PathParams.SimFrequency = 30.f;
	PathParams.StartLocation = GetActorLocation();
	PathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
	PathParams.ActorsToIgnore.Add(this);
 
	FPredictProjectilePathResult PathResult;
 
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
	*/
}
 
 void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
 {
 	AMainCharacter* OwnerCharacter = Cast<AMainCharacter>(GetOwner());
 	if (OwnerCharacter)
 	{
 		AMyPlayerController* OwnerController = Cast<AMyPlayerController>(OwnerCharacter->Controller);
 		if (OwnerController)
 		{
 			if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
 			{
 				UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
 				Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
 				return;
 			}
 			AMainCharacter* HitCharacter = Cast<AMainCharacter>(OtherActor);
 			if (bUseServerSideRewind && OwnerCharacter->GetLagCompensation() && OwnerCharacter->IsLocallyControlled() && HitCharacter)
 			{
 				OwnerCharacter->GetLagCompensation()->ProjectileServerScoreRequest(
					 HitCharacter,
					 TraceStart,
					 InitialVelocity,
					 OwnerController->GetServerTime() - OwnerController->SingleTripTime
				 );
 			}
 		}
 	}
 
 	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
 }


#if WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);
 
	FName PropertyName = Event.Property != nullptr ? Event.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitialSpeed))
	{
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->InitialSpeed = InitialSpeed;
			ProjectileMovementComponent->MaxSpeed = InitialSpeed;
		}
	}
}
#endif