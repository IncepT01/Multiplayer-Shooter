// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

#include "NetworkMessage.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/BaseWeapon/BaseWeapon.h"
#include "MultiplayerShooter/ActorComponents/CombatComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MultiplayerShooter/MultiplayerShooter.h"
#include "MultiplayerShooter/PlayerController/MyPlayerController.h"
#include "MultiplayerShooter/GameMode/MainGameMode.h"
#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Components/EditableTextBox.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/SphereComponent.h"
#include "MultiplayerShooter/PlayerState/MainPlayerState.h"
#include "MultiplayerShooter/ActorComponents/CombatComponent.h"
#include "MultiplayerShooter/ActorComponents/LagCompensationComponent.h"
#include "MultiplayerShooter/ActorComponents/BuffComponent.h"
#include "MultiplayerShooter/HUD/Chat.h"
#include "Components/Widget.h"
#include "MultiplayerShooter/ActorComponents/ChatComponent.h"

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

	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	Chat = CreateDefaultSubobject<UChatComponent>(TEXT("ChatComponent"));
	Chat->SetIsReplicated(true);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 720.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);

	/** 
 	* Hit boxes for server-side rewind
 	*/
	
 	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
 	head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), head);
 
 	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
 	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);
 
 	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
 	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);
 
 	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
 	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);
 
 	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
 	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);
 
 	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
 	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);
 
 	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
 	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);
 
 	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
 	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);
 
 	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
 	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);
 
 	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
 	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);
 
 	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
 	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);
 
 	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
 	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);
 
 	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
 	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);
 
 	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
 	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);
 
 	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
 	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);
 
 	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
 	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value){
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AMainCharacter::Jump);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMainCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMainCharacter::LookUp);

	PlayerInputComponent->BindAction("Chat", IE_Pressed,this, &AMainCharacter::ChatButtonPressed);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AMainCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMainCharacter::CrouchButtonPressed);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMainCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMainCharacter::AimButtonReleased);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMainCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMainCharacter::FireButtonReleased);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AMainCharacter::ReceiveDamage);
	}

	//Spawn Starter weapon
	if (StarterWeapon && GetWorld())
	{
		FVector SpawnLocation(0.0f, 0.0f, 0.0f);
		FRotator SpawnRotation(0.0f, 0.0f, 0.0f);

		if (HasAuthority())
		{
			OverlappingWeapon = GetWorld()->SpawnActor<ABaseWeapon>(StarterWeapon, SpawnLocation, SpawnRotation);

			if (OverlappingWeapon)
			{
				AMainCharacter::EquipButtonPressed();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to spawn weapon!"));
			}
		}
	}
}

void AMainCharacter::MoveForward(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void AMainCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;
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

	//Calling CombatComponent's Tick manually
	if (Combat)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Manually calling CombatComponent Tick!"));
		Combat->TickComponent(DeltaTime, LEVELTICK_All, nullptr);
	}

	RotateInPlace(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();

	//UE_LOG(LogTemp, Warning, TEXT("DamageMul: %f!"), DamageMultiplier);
}

void AMainCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}


void AMainCharacter::PollInit()
{
	if (MainPlayerState == nullptr)
	{
		MainPlayerState = GetPlayerState<AMainPlayerState>();
		if (MainPlayerState)
		{
			MainPlayerState->AddToScore(0.f);
		}
	}
}


void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMainCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AMainCharacter, Health);
	DOREPLIFETIME(AMainCharacter, bDisableGameplay);
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
	if (bDisableGameplay) return;
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

void AMainCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
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
		Combat->BaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		Combat->AimWalkSpeed = GetCharacterMovement()->MaxWalkSpeed * 0.8f;
	}

	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<AMyPlayerController>(Controller);
		}
	}

	if (Buff)
	{
		Buff->Character = this;
		Buff->SetInitialSpeeds(
			 Combat->BaseWalkSpeed, 
			 Combat->AimWalkSpeed
		 );
	}
}

void AMainCharacter::AimButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void AMainCharacter::AimButtonReleased()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

bool AMainCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

void AMainCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still, not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void AMainCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

float AMainCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

ABaseWeapon* AMainCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

void AMainCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}

	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

inline void AMainCharacter::Jump()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AMainCharacter::FireButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void AMainCharacter::FireButtonReleased()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void AMainCharacter::ChatButtonPressed()
{
	MyPlayerController = MyPlayerController == nullptr
									 ? Cast<AMyPlayerController>(Controller)
									 : MyPlayerController;
	if (MyPlayerController)
	{
		AMainHUD* MainHUD = Cast<AMainHUD>(MyPlayerController->GetHUD());

		if (MainHUD)
		{
			// Assuming you already have a reference to your UMG widget, let's say ChatWidget is your widget
			UChat* ChatWidget = MainHUD->Chat;
			if (ChatWidget)
			{
				if (ChatWidget->InputField)
				{
					// Check if the input field has focus
					if (ChatWidget->InputField->HasUserFocus(MyPlayerController))
					{
						UE_LOG(LogTemp, Display, TEXT("Chat Input was in focus -> removing focus"));
		
						// Unfocus and re-enable player input
						MyPlayerController->SetInputMode(FInputModeGameOnly());
						MyPlayerController->bShowMouseCursor = false;
					}
					else
					{
						UE_LOG(LogTemp, Display, TEXT("Chat Input was not in focus -> focusing now"));

						// Focus input field and disable movement/game input
						ChatWidget->InputField->SetFocus();
						MyPlayerController->SetInputMode(FInputModeUIOnly());
						MyPlayerController->bShowMouseCursor = true;
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("No ChatWidget"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("No MainHUD"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("No PlayerController"));
	}
	UE_LOG(LogTemp, Display, TEXT("ChatButtonPressed"));
}

void AMainCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                   class AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	
	if (HasAuthority())
	{
		UpdateHUDHealth();
		PlayHitReactMontage();
	}
	//This call here does not update on clients
	//PlayHitReactMontage();

	if (Health <= 0.f)
	{
		AMainGameMode* MainGameMode = GetWorld()->GetAuthGameMode<AMainGameMode>();
		if (MainGameMode)
		{
			MyPlayerController = MyPlayerController == nullptr
				                     ? Cast<AMyPlayerController>(Controller)
				                     : MyPlayerController;
			AMyPlayerController* AttackerController = Cast<AMyPlayerController>(InstigatorController);
			MainGameMode->PlayerEliminated(this, MyPlayerController, AttackerController);
		}
	}
}

///Elimination

void AMainCharacter::Elim()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}
	Multicast_Elim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&AMainCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void AMainCharacter::Multicast_Elim_Implementation()
{
	if (MyPlayerController)
	{
		MyPlayerController->SetHUDWeaponAmmo(0);
	}

	if (bElimmed == false)
	{
		bElimmed = true;
		PlayElimMontage();
	}

	//Disable Character Movement
	bDisableGameplay = true;
	if (Combat)
	{
		if (Combat->bFireButtonPressed == true)
		{
			Combat->FireButtonPressed(false);
		}
	}
	
	// Disable collision
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMainCharacter::ElimTimerFinished()
{
	AMainGameMode* MainGameMode = GetWorld()->GetAuthGameMode<AMainGameMode>();
	if (MainGameMode)
	{
		MainGameMode->RequestRespawn(this, Controller);
	}
}

///End elimination

FVector AMainCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}


void AMainCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void AMainCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AMainCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && HitReactMontage)
		{
			AnimInstance->Montage_Play(HitReactMontage);
			FName SectionName = ("FromFront");
			AnimInstance->Montage_JumpToSection(SectionName);
			//UE_LOG(LogTemp, Warning, TEXT("Playing hit react Montage"))
		}
	}
}


void AMainCharacter::PlayElimMontage()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Montage start!"))
		AnimInstance->Montage_Play(ElimMontage);
	}
}


void AMainCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void AMainCharacter::UpdateHUDHealth()
{
	MyPlayerController = MyPlayerController == nullptr ? Cast<AMyPlayerController>(Controller) : MyPlayerController;
	if (MyPlayerController)
	{
		MyPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void AMainCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

//Replication only happens on clients!
void AMainCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health > 0 && Health < LastHealth)
	{
		PlayHitReactMontage();
	}
}

void AMainCharacter::MulticastFire_Implementation()
{
	PlayFireMontage(IsAiming());
}
