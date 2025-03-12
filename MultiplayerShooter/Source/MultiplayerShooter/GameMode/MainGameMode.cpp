// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "MultiplayerShooter/PlayerController/MYPlayerController.h"
#include "Kismet/GameplayStatics.h"
 #include "GameFramework/PlayerStart.h"
 
 void AMainGameMode::PlayerEliminated(class AMainCharacter* ElimmedCharacter, class AMyPlayerController* VictimController, AMyPlayerController* AttackerController)
 {
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
