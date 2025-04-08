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

	if (!canGetPoints){return;}
 	
 	SetScore(GetScore() + ScoreAmount);
 	Character = Character == nullptr ? Cast<AMainCharacter>(GetPawn()) : Character;
 	if (Character)
 	{
 		Controller = Controller == nullptr ? Cast<AMyPlayerController>(Character->Controller) : Controller;
 		if (Controller)
 		{
 			Controller->SetHUDScore(GetScore());
 		}
 	}

 	//A terrible, horrible, no good, very bad solution
 	canGetPoints = false;
 	GetWorld()->GetTimerManager().SetTimer(TimerHandle_CanGetPoints, this, &AMainPlayerState::ResetCanGetPoints, 1.0f, false);
 }

void AMainPlayerState::ResetCanGetPoints()
 {
 	canGetPoints = true;
 }
 
void AMainPlayerState::OnRep_Score()
 {
 	Super::OnRep_Score();
 
 	Character = Character == nullptr ? Cast<AMainCharacter>(GetPawn()) : Character;
 	if (Character)
 	{
 		Controller = Controller == nullptr ? Cast<AMyPlayerController>(Character->Controller) : Controller;
 		if (Controller)
 		{
 			Controller->SetHUDScore(GetScore());
 		}
 	}
 }

