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
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Math/UnitConversion.h"
#include "MultiplayerShooter/PlayerController/MyPlayerController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("UCombatComponent BeginPlay Called!"));
	
	PrimaryComponentTick.SetTickFunctionEnable(true);
	SetComponentTickEnabled(true);
	
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

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
			//DrawDebugSphere(GetWorld(), Start, 16.f, 12, FColor::Red);
		}
 
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

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);

		if(Character->HasAuthority())
		{
			if (EquippedWeapon)
			{
				UE_LOG(LogTemp,Warning, TEXT("Setting ammo on server in Combat tick"));
				EquippedWeapon->SetHUDAmmo();
			}
		}
	}
	
}

void UCombatComponent::EquipWeapon(ABaseWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;

	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, FString(TEXT("Gun picked up")));
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, FString(TEXT("No hand socket found")));
	}
	//UE_LOG(LogTemp, Warning, TEXT("Setting owner to: %p"), *Character->GetName())
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDAmmo();
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

	if (Character->IsLocallyControlled())
	{
		bAimButtonPressed = bIsAiming;
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
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}
		
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::Fire()
{
	if (CanFire())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Firing (CombatComponent.cpp/Fire)"));
		EquippedWeapon->SetHUDAmmo();
		bCanFire = false;
		Server_Fire(bFireButtonPressed, HitTarget);
		LocalFire(HitTarget);
		if (EquippedWeapon)
		{
			CrosshairShootingFactor = .75f;
		}
		StartFireTimer();
	}
	
}

void UCombatComponent::LocalFire(const FVector_NetQuantize& fnHitTarget)
{
	//UE_LOG(LogTemp, Warning, TEXT("Round spent in local fire"));
	EquippedWeapon->Fire(fnHitTarget);
	EquippedWeapon->SpendRound();
	if (EquippedWeapon->MuzzleFlashComponent)
	{
		EquippedWeapon->MuzzleFlashComponent->Deactivate(); // Deactivate the Niagara system
		EquippedWeapon->MuzzleFlashComponent->DestroyComponent(); // Destroy the component
		EquippedWeapon->MuzzleFlashComponent = nullptr;
		//UE_LOG(LogTemp, Warning, TEXT("MuzzleFlash stopped"));
	}
	if (EquippedWeapon->MuzzleFlashNiagaraSystem && EquippedWeapon->WeaponMesh)
	{
		// Spawn the Niagara particle system at the muzzle socket and set it to loop
		//UE_LOG(LogTemp, Warning, TEXT("MuzzleFlash spawning"));
		EquippedWeapon->MuzzleFlashComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			EquippedWeapon->MuzzleFlashNiagaraSystem,
			EquippedWeapon->WeaponMesh,
			"Muzzle", // Socket name
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}

	if (EquippedWeapon->FireSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Playing FireSound"));
		UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->FireSound, EquippedWeapon->GetActorLocation());
		//UGameplayStatics::PlaySound2D(this, EquippedWeapon->FireSound);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No FireSound"));
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	
	Fire();
	
}

void UCombatComponent::Server_Fire_Implementation(bool bLocalFireButtonPressed, const FVector_NetQuantize& TracerTarget)
{
	//bLocalFireButtonPressed means it is used in this Function only
	//UE_LOG(LogTemp, Warning, TEXT("Firing (CombatComponent.cpp/Server_Fire)"));
	//UE_LOG(LogTemp, Display, TEXT("Server bLocalFIreButtonPressed %hs"), bLocalFireButtonPressed ? "true" : "false");
	if (bLocalFireButtonPressed)
	{
		EquippedWeapon->Multicast_StartFiring(TracerTarget);
		if (Character)
		{
			//Enable to player Fire Montage
			Character->MulticastFire();
			EquippedWeapon->SetHUDAmmo();
		}
	}
	else
	{
		//EquippedWeapon->Multicast_StopFiring();
	}
}

void UCombatComponent::Multicast_StartFiring_Implementation(const FVector& fnHitTarget)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority())
	{
		return;
	}

	LocalFire(fnHitTarget);
		
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
 
			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}
 
			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);
 
			HUDPackage.CrosshairSpread = 
				0.5f + 
				CrosshairVelocityFactor + 
				CrosshairInAirFactor -
				CrosshairAimFactor +
				CrosshairShootingFactor;
			
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::OnRep_Aiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		bAiming = bAimButtonPressed;
	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;
 
	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeapon->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr) return;
	bCanFire = true;
	if (bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;
	return !EquippedWeapon->IsEmpty() && bCanFire;
}


