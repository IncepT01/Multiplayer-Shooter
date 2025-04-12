// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "Announcement.h"
#include "Chat.h"
#include "Components/TextBlock.h"

void AMainHUD::BeginPlay()
 {
 	Super::BeginPlay();
 }
 
void AMainHUD::AddCharacterOverlay()
 {
	if (CharacterOverlay)
	{
		return;
	}
	
 	APlayerController* PlayerController = GetOwningPlayerController();
 	if (PlayerController && CharacterOverlayClass)
 	{
 		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
 		if (CharacterOverlay)
 		{
 			//UE_LOG(LogTemp, Warning, TEXT("Creating CharacterOverlay Widget and adding it to viewport"));
 			CharacterOverlay->AddToViewport();
 		}
 		else
 		{
 			//UE_LOG(LogTemp, Warning, TEXT("Failed to create CharacterOverlay Widget"));
 		}
 	}
 }

void AMainHUD::DrawHUD()
{
	Super::DrawHUD();
 
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;
		
		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f,0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled,0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled,0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f,-SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter,	Spread);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f,SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread);
		}
	}
}


void AMainHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);
 
	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		FLinearColor::White
	);
}

void AMainHUD::AddAnnouncement()
 {

	if (Announcement)
	{
		return;
	}
 	APlayerController* PlayerController = GetOwningPlayerController();
 	if (PlayerController && AnnouncementClass)
 	{
 		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
 		if (Announcement)
 		{
 			Announcement->AddToViewport();
 			//UE_LOG(LogTemp, Warning, TEXT("Announcement widget successfully created and added to viewport"));
 		}
 		else
 		{
 			//UE_LOG(LogTemp, Warning, TEXT("Failed to create Announcement widget"));
 		}
 	}
 }

void AMainHUD::AddChat()
{

	if (Chat)
	{
		return;
	}
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && ChatClass)
	{
		Chat = CreateWidget<UChat>(PlayerController, ChatClass);
		if (Chat)
		{
			Chat->AddToViewport();
			//UE_LOG(LogTemp, Warning, TEXT("Announcement widget successfully created and added to viewport"));
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Failed to create Announcement widget"));
		}
	}
}
