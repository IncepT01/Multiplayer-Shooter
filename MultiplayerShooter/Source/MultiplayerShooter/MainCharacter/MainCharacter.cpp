// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/BaseWeapon/BaseWeapon.h"
#include "MultiplayerShooter/ActorComponents/CombatComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	static ConstructorHelpers::FClassFinder<ABaseWeapon> WeaponFinder(TEXT("BlueprintGeneratedClass'/Game/Blueprints/BaseWeapon/BP_BaseWeapon.BP_BaseWeapon_C'"));
	if (WeaponFinder.Succeeded())
	{
		WeaponBlueprintClass = WeaponFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon Blueprint not found!"));
	}

	
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMainCharacter::LookUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMainCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::CrouchButtonPressed);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponBlueprintClass && GetWorld()) // Check if class and world exist
	{
		FVector SpawnLocation(0.0f, 0.0f, 0.0f);
		FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

		OverlappingWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponBlueprintClass, SpawnLocation, SpawnRotation);

		if (OverlappingWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon spawned successfully!"));
			AMainCharacter::EquipButtonPressed();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn weapon!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponClassFinder.Class is NULL or GetWorld() is not valid!"));
	}
}

void AMainCharacter::MoveForward(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AMainCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMainCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void AMainCharacter::SetOverlappingWeapon(ABaseWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}


void AMainCharacter::OnRep_OverlappingWeapon(ABaseWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void AMainCharacter::EquipButtonPressed()
{
	if (Combat)
	{
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void AMainCharacter::CrouchButtonPressed() {
	if (bIsCrouched) {
		UnCrouch();
	}
	else {
		Crouch();
	}
}

void AMainCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

bool AMainCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}

	
}
