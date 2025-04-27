// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MultiplayerShooter/HUD/MainHUD.h"
#include "MultiplayerShooter/PlayerController/MyPlayerController.h"


void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	SetupInput();
}

void ALobbyGameMode::SetupInput()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	
	if (PlayerController)
	{
		UInputComponent* InputComponentLocal = PlayerController->InputComponent;
		if (InputComponentLocal)
		{
			InputComponentLocal->BindAction("DebugKey0", IE_Pressed, this, &ALobbyGameMode::TravelToLevel);
		}
	}
}

void ALobbyGameMode::TravelToLevel()
{
	UWorld* World = GetWorld();
	if (World)
	{
		bUseSeamlessTravel = true;
		World->ServerTravel(FString("/Game/Maps/Level1?listen"));
	}
}



