// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
 
 
void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();
 
}


void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);
 
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;
	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;
}
 
void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsElimmed()) return;
 
	const float HealThisFrame = HealingRate * DeltaTime;
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth()));
	Character->UpdateHUDHealth();
	AmountToHeal -= HealThisFrame;
 
	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0.f;
	}
}

void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed;
	InitialCrouchSpeed = CrouchSpeed;
}

void UBuffComponent::BuffDamage(float DamageMul, float BuffTime)
{
	if (Character == nullptr) return;
 
	Character->GetWorldTimerManager().SetTimer(
		DamageBuffTimer,
		this,
		&UBuffComponent::ResetDamage,
		BuffTime
	);
	
	Character->SetDamageMultiplier(DamageMul);
	MulticastDamageBuff(DamageMul);
}

void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime)
{
	if (Character == nullptr) return;
	
	Character->GetWorldTimerManager().SetTimer(
		SpeedBuffTimer,
		this,
		&UBuffComponent::ResetSpeeds,
		BuffTime
	);
 
	if (Character->GetCharacterMovement())
	{
		Character->GetCombat()->BaseWalkSpeed = InitialBaseSpeed + BuffBaseSpeed;
		Character->GetCombat()->AimWalkSpeed = InitialCrouchSpeed + BuffCrouchSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed + BuffBaseSpeed;
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed + BuffCrouchSpeed;
		
	}
	MulticastSpeedBuff(Character->GetCharacterMovement()->MaxWalkSpeed, Character->GetCharacterMovement()->MaxWalkSpeedCrouched);
}
 
void UBuffComponent::ResetSpeeds()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
 
	Character->GetCombat()->BaseWalkSpeed = InitialBaseSpeed;
	Character->GetCombat()->AimWalkSpeed = InitialCrouchSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed;
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
}

void UBuffComponent::ResetDamage()
{
	Character->SetDamageMultiplier(1.f);
	MulticastDamageBuff(1.f);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed)
{
	Character->GetCombat()->BaseWalkSpeed = BaseSpeed;
	Character->GetCombat()->AimWalkSpeed = CrouchSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
}

void UBuffComponent::MulticastDamageBuff_Implementation(float DamageMul)
{
	Character->SetDamageMultiplier(DamageMul);
}