// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "MultiplayerShooter/BaseWeapon/BaseWeapon.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
 #include "DrawDebugHelpers.h"
#include "Math/UnitConversion.h"
#include "MultiplayerShooter/PlayerController/MyPlayerController.h"
 #include "MultiplayerShooter//HUD/MainHUD.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 300.f;
	

}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<AMainCharacter>(GetOwner());
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("CombatComponent: Character is NULL!"));
		return;
	}

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}

	//UE_LOG(LogTemp, Warning, TEXT("UCombatComponent BeginPlay Called!"));

	// Manually enable ticking if needed
	PrimaryComponentTick.SetTickFunctionEnable(true);
	SetComponentTickEnabled(true);

	// Manually register component
	if (!IsRegistered())
	{
		RegisterComponent();
		UE_LOG(LogTemp, Warning, TEXT("CombatComponent was not registered, now it is!"));
	}
}



void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Traceing!"));
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
 
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
 
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
 
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
 
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		if (!TraceHitResult.bBlockingHit)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Nothing is hit!"));
			TraceHitResult.ImpactPoint = End;
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("DrawingSphere"));
			/*
			DrawDebugSphere(
				GetWorld(),
				TraceHitResult.ImpactPoint,
				12.f,
				12,
				FColor::Red
			);
			*/
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!IsComponentTickEnabled())
	{
		UE_LOG(LogTemp, Error, TEXT("CombatComponent: Tick is DISABLED!"));
	}
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//FHitResult HitResult;
	//TraceUnderCrosshairs(HitResult);

	SetHUDCrosshairs(DeltaTime);
	
}

void UCombatComponent::EquipWeapon(ABaseWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, FString(TEXT("Gun picked up")));
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, FString(TEXT("No hand socket found")));
	}
	EquippedWeapon->SetOwner(Character);
	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	FHitResult HitResult;
	if (bFireButtonPressed)
	{
		TraceUnderCrosshairs(HitResult);
	}
	
	Server_Fire(bFireButtonPressed, HitResult.ImpactPoint);
}

void UCombatComponent::Server_Fire_Implementation(bool bLocalFireButtonPressed, const FVector_NetQuantize& TracerTarget)
{
	//bLocalFireButtonPressed means it is used in this Function only
	UE_LOG(LogTemp, Display, TEXT("Server bLocalFIreButtonPressed %hs"), bLocalFireButtonPressed ? "true" : "false");
	if (bLocalFireButtonPressed)
	{
		EquippedWeapon->Multicast_StartFiring(TracerTarget);
	}
	else
	{
		EquippedWeapon->Multicast_StopFiring();
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;
 
	Controller = Controller == nullptr ? Cast<AMyPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<AMainHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			FHUDPackage HUDPackage;
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
			}
			//Spread calculation

			// [0, 600] -> [0, 1]
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;
 
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
 
			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}
 
			HUDPackage.CrosshairSpread = CrosshairVelocityFactor + CrosshairInAirFactor;
			
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}