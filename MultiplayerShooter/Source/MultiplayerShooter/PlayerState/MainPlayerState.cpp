// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerState.h"
 #include "MultiplayerShooter/MainCharacter/MainCharacter.h"
 #include "MultiplayerShooter/PlayerController/MyPlayerController.h"
 #include "Net/UnrealNetwork.h"
 
 void AMainPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
 {
 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 	
 }
 
void AMainPlayerState::AddToScore(float ScoreAmount)
 {
 	SetScore(GetScore() + ScoreAmount);
 	Character = Character == nullptr ? Cast<AMainCharacter>(GetPawn()) : Character;
 	if (Character && Character->Controller)
 	{
 		Controller = Controller == nullptr ? Cast<AMyPlayerController>(Character->Controller) : Controller;
 		if (Controller)
 		{
 			Controller->SetHUDScore(GetScore());
 		}
 	}
 }
 
void AMainPlayerState::OnRep_Score()
 {
 	Super::OnRep_Score();
 
 	Character = Character == nullptr ? Cast<AMainCharacter>(GetPawn()) : Character;
 	if (Character && Character->Controller)
 	{
 		Controller = Controller == nullptr ? Cast<AMyPlayerController>(Character->Controller) : Controller;
 		if (Controller)
 		{
 			Controller->SetHUDScore(GetScore());
 		}
 	}
 }

