// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MultiplayerShooter/HUD/MainHUD.h"
 #include "MultiplayerShooter//HUD/CharacterOverlay.h"
 #include "Components/ProgressBar.h"
 #include "Components/TextBlock.h"
 
 void AMyPlayerController::BeginPlay()
 {
 	Super::BeginPlay();
 
 	BlasterHUD = Cast<AMainHUD>(GetHUD());
 }
 
void AMyPlayerController::SetHUDHealth(float Health, float MaxHealth)
 {
 	BlasterHUD = BlasterHUD == nullptr ? Cast<AMainHUD>(GetHUD()) : BlasterHUD;
 
 	bool bHUDValid = BlasterHUD && 
		 BlasterHUD->CharacterOverlay && 
		 BlasterHUD->CharacterOverlay->HealthBar && 
		 BlasterHUD->CharacterOverlay->HealthText;
 	if (bHUDValid)
 	{
 		const float HealthPercent = Health / MaxHealth;
 		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
 		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
 		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
 	}
 }
