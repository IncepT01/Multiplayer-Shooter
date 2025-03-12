// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h" 
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"


// Sets default values
ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	//Reenable collision only on the server
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnSphereEndOverlap);
	}

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}

void ABaseWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
	if (Character)
	{
		Character->SetOverlappingWeapon(this);
	}
}

void ABaseWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainCharacter* Character = Cast<AMainCharacter>(OtherActor);
	if (Character)
	{
		Character->SetOverlappingWeapon(nullptr);
	}
}

void ABaseWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, WeaponState);
}

void ABaseWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void ABaseWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		break;
	}
}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseWeapon::Multicast_StartFiring_Implementation(const FVector& HitTarget)
{
	if (MuzzleFlashComponent)
	{
		MuzzleFlashComponent->Deactivate();  // Deactivate the Niagara system
		MuzzleFlashComponent->DestroyComponent();  // Destroy the component
		MuzzleFlashComponent = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("MuzzleFlash stopped"));
	}
	if (MuzzleFlashNiagaraSystem && WeaponMesh)
	{
		// Spawn the Niagara particle system at the muzzle socket and set it to loop
		UE_LOG(LogTemp, Warning, TEXT("MuzzleFlash spawning"));
		MuzzleFlashComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			MuzzleFlashNiagaraSystem,
			WeaponMesh,
			"Muzzle",  // Socket name
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}
	

}

void ABaseWeapon::Multicast_StopFiring_Implementation()
{
	if (MuzzleFlashComponent)
	{
		MuzzleFlashComponent->Deactivate();  // Deactivate the Niagara system
		MuzzleFlashComponent->DestroyComponent();  // Destroy the component
		MuzzleFlashComponent = nullptr;
	}
}