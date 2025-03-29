// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "MultiplayerShooter/PlayerController/MYPlayerController.h"
#include "Kismet/GameplayStatics.h"
 #include "GameFramework/PlayerStart.h"
#include "MultiplayerShooter/PlayerState/MainPlayerState.h"

void AMainGameMode::PlayerEliminated(class AMainCharacter* ElimmedCharacter, class AMyPlayerController* VictimController, AMyPlayerController* AttackerController)
 {
 	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
 	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
 	AMainPlayerState* AttackerPlayerState = AttackerController ? Cast<AMainPlayerState>(AttackerController->PlayerState) : nullptr;
 	AMainPlayerState* VictimPlayerState = VictimController ? Cast<AMainPlayerState>(VictimController->PlayerState) : nullptr;
 
 	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
 	{
 		AttackerPlayerState->AddToScore(1.f);
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
