// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MultiplayerShooter/HUD/MainHUD.h"
 #include "MultiplayerShooter//HUD/CharacterOverlay.h"
 #include "Components/ProgressBar.h"
 #include "Components/TextBlock.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
 
 void AMyPlayerController::BeginPlay()
 {
 	Super::BeginPlay();
 
 	MainHUD = Cast<AMainHUD>(GetHUD());
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

/*
void AMyPlayerController::SetHUDHealth(float Health, float MaxHealth)
 {
 	MainHUD = MainHUD == nullptr ? Cast<AMainHUD>(GetHUD()) : MainHUD;
 
 	bool bHUDValid = MainHUD && 
		 MainHUD->CharacterOverlay && 
		 MainHUD->CharacterOverlay->HealthBar && 
		 MainHUD->CharacterOverlay->HealthText;
 	if (bHUDValid)
 	{
 		const float HealthPercent = Health / MaxHealth;
 		//MainHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
 		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
 		MainHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
 	}
 }
*/

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
