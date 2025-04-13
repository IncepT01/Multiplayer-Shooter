// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"

#include "ModuleDescriptor.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "MultiplayerShooter/PlayerController/MYPlayerController.h"
#include "Kismet/GameplayStatics.h"
 #include "GameFramework/PlayerStart.h"
#include "MultiplayerShooter/GameState/MainGameState.h"
#include "MultiplayerShooter/PlayerState/MainPlayerState.h"
#include "GameFramework/GameSession.h"
#include "MultiplayerShooter/Database/BuffDatabaseManager.h"
#include "MultiplayerShooter/Pickups/HealthPickup.h"


namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

AMainGameMode::AMainGameMode()
{
	bDelayedStart = true;
}
 
void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();
 
	LevelStartingTime = GetWorld()->GetTimeSeconds();

	if (BackgroundMusic)
	{
		//MusicAudioComponent = UGameplayStatics::SpawnSound2D(this, BackgroundMusic);
	}
	
	//The server should read the buffs
	if (HasAuthority())
	{
		BuffList = UBuffDatabaseManager::Get()->LoadBuffs();

		for (auto i : BuffList)
		{
			UE_LOG(LogTemp, Warning, TEXT("Buff Name: %s, Amount: %f"), *i.Key,i.Value);
		}
	}
	
}

void AMainGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AMyPlayerController* MyPC = Cast<AMyPlayerController>(NewPlayer);
	if (MyPC)
	{
		MyPC->HighPingDelegate.AddDynamic(this, &AMainGameMode::HandleHighPing);
		UE_LOG(LogTemp, Warning, TEXT("Subscribed to HighPingDelegate for: %s"), *MyPC->GetName());
	}
}
 
void AMainGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
 
	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void AMainGameMode::PlayerEliminated(class AMainCharacter* ElimmedCharacter, class AMyPlayerController* VictimController, AMyPlayerController* AttackerController)
 {
 	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
 	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
 	AMainPlayerState* AttackerPlayerState = AttackerController ? Cast<AMainPlayerState>(AttackerController->PlayerState) : nullptr;
 	AMainPlayerState* VictimPlayerState = VictimController ? Cast<AMainPlayerState>(VictimController->PlayerState) : nullptr;

	AMainGameState* MainGameState = GetGameState<AMainGameState>();
 	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && MainGameState)
 	{
 		AttackerPlayerState->AddToScore(1.f);
 		MainGameState->UpdateTopScore(AttackerPlayerState);
 	}
	
 	 
 	
 	if (ElimmedCharacter)
 	{
 		ElimmedCharacter->Elim();
 	}
 }

void AMainGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
 {
 	if (ElimmedCharacter)
 	{
 		ElimmedCharacter->Reset();
 		ElimmedCharacter->Destroy();
 	}
 	if (ElimmedController)
 	{
 		TArray<AActor*> PlayerStarts;
 		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
 		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
 		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
 	}
 }

void AMainGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
 
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AMyPlayerController* Player = Cast<AMyPlayerController>(*It);
		if (Player)
		{
			Player->OnMatchStateSet(MatchState);
		}
	}
}

void AMainGameMode::HandleHighPing(bool bHighPing, APlayerController* PlayerController)
{
	if (bHighPing && PlayerController)
	{
		GameSession = GetWorld()->GetAuthGameMode()->GameSession;
        
		if (GameSession)
		{
			UE_LOG(LogTemp, Warning, TEXT("Kicked player %s due to high ping."), *PlayerController->GetName());
			//This line will kick the player!
			//GameSession->KickPlayer(PlayerController, FText::FromString(TEXT("KickPlayer")));
		}
	}
}

