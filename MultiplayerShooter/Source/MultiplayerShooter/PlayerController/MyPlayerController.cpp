// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MultiplayerShooter/HUD/MainHUD.h"
 #include "MultiplayerShooter//HUD/CharacterOverlay.h"
 #include "Components/ProgressBar.h"
 #include "Components/TextBlock.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "Net/UnrealNetwork.h"
 
 void AMyPlayerController::BeginPlay()
 {
 	Super::BeginPlay();
 
 	MainHUD = Cast<AMainHUD>(GetHUD());
 }

void AMyPlayerController::Tick(float DeltaTime)
 {
 	Super::Tick(DeltaTime);
 
 	SetHUDTime();
 	CheckTimeSync(DeltaTime);
 }

void AMyPlayerController::CheckTimeSync(float DeltaTime)
 {
 	TimeSyncRunningTime += DeltaTime;
 	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
 	{
 		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
 		TimeSyncRunningTime = 0.f;
 	}
 }

void AMyPlayerController::OnPossess(APawn* InPawn)
 {
 	Super::OnPossess(InPawn);
 	AMainCharacter* MainCharacter = Cast<AMainCharacter>(InPawn);
 	if (MainCharacter)
 	{
 		SetHUDHealth(MainCharacter->GetHealth(), MainCharacter->GetMaxHealth());
 	}
 }

void AMyPlayerController::SetHUDHealth(float Health, float MaxHealth)
 {
 	if (!IsValid(MainHUD))
 	{
 		MainHUD = Cast<AMainHUD>(GetHUD());
 	}
 	
 	if (IsValid(MainHUD) && IsValid(MainHUD->CharacterOverlay) && IsValid(MainHUD->CharacterOverlay->HealthText))
 	{
 		const float HealthPercent = Health / MaxHealth;
 		
 		FString HealthText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Health));
 		MainHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
 		
 		// MainHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
 	}
 	else
 	{
 		UE_LOG(LogTemp, Warning, TEXT("HUD components are invalid, setting timer to retry."));
 	}
 	
 }

void AMyPlayerController::SetHUDScore(float Score)
 {
 	if (!IsValid(MainHUD))
 	{
 		MainHUD = Cast<AMainHUD>(GetHUD());
 	}
 	
 	if (IsValid(MainHUD) && IsValid(MainHUD->CharacterOverlay) && IsValid(MainHUD->CharacterOverlay->ScoreAmount))
 	{
 		FString ScoreText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Score));
 		MainHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));

 	}
 	else
 	{
 		UE_LOG(LogTemp, Warning, TEXT("HUD components are invalid, setting timer to retry."));
 	}
 }

void AMyPlayerController::SetHUDWeaponAmmo(int32 Ammo)
 {
 	if (!IsValid(MainHUD))
 	{
 		MainHUD = Cast<AMainHUD>(GetHUD());
 	}
 	
 	if (IsValid(MainHUD) && IsValid(MainHUD->CharacterOverlay) && IsValid(MainHUD->CharacterOverlay->WeaponAmmoAmount))
 	{
 		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
 		MainHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));

 	}
 	else
 	{
 		UE_LOG(LogTemp, Warning, TEXT("HUD components are invalid, setting timer to retry."));
 	}
 }

void AMyPlayerController::SetHUDMatchCountdown(float CountdownTime)
 {
 	if (!IsValid(MainHUD))
 	{
 		MainHUD = Cast<AMainHUD>(GetHUD());
 	}
 	
 	if (IsValid(MainHUD) && IsValid(MainHUD->CharacterOverlay) && IsValid(MainHUD->CharacterOverlay->MatchCountdownText))
 	{
 		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
 		int32 Seconds = CountdownTime - Minutes * 60;
 
 		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
 		MainHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));

 	}
 	else
 	{
 		UE_LOG(LogTemp, Warning, TEXT("HUD components are invalid, setting timer to retry."));
 	}
 }


void AMyPlayerController::SetHUDTime()
 {
 	uint32 SecondsLeft = FMath::CeilToInt(MatchTime - GetServerTime());
 	if (CountdownInt != SecondsLeft)
 	{
 		SetHUDMatchCountdown(MatchTime - GetServerTime());
 	}
 
 	CountdownInt = SecondsLeft;
 }

void AMyPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
 {
 	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
 	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
 }
 
void AMyPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
 {
 	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
 	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
 	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
 }
 
float AMyPlayerController::GetServerTime()
 {
 	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
 	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
 }
 
void AMyPlayerController::ReceivedPlayer()
 {
 	Super::ReceivedPlayer();
 	if (IsLocalController())
 	{
 		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
 	}
 }