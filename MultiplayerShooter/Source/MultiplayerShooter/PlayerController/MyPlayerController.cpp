// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MultiplayerShooter/HUD/MainHUD.h"
#include "MultiplayerShooter//HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerShooter/GameMode/MainGameMode.h"
#include "MultiplayerShooter/ActorComponents/CombatComponent.h"
#include "MultiplayerShooter/BaseWeapon/BaseWeapon.h"
#include "MultiplayerShooter/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnitConversion.h"
#include "MultiplayerShooter/GameState/MainGameState.h"
#include "MultiplayerShooter/PlayerState/MainPlayerState.h"

void AMyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
	DOREPLIFETIME(AMyPlayerController, MatchState);
}

 void AMyPlayerController::BeginPlay()
 {
 	Super::BeginPlay();

	
 	MainHUD = Cast<AMainHUD>(GetHUD());

	/*
	GetWorldTimerManager().SetTimer(
		CheckMatchStateTimerHandle, 
		this, 
		&AMyPlayerController::ServerCheckMatchState, 
		0.5f, 
		false
	);*/
	
	//ServerCheckMatchState();

	ServerTryCheckMatchState();
 }

void AMyPlayerController::Tick(float DeltaTime)
 {
 	Super::Tick(DeltaTime);
 
 	SetHUDTime();
 	CheckTimeSync(DeltaTime);
	PollInit();
	CheckPing(DeltaTime);
 }

void AMyPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (MainHUD && MainHUD->CharacterOverlay)
		{
			CharacterOverlay = MainHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
			}
		}
	}
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
 		bInitializeCharacterOverlay = true;
 		HUDHealth = Health;
 		HUDMaxHealth = MaxHealth;
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
 		bInitializeCharacterOverlay = true;
 		HUDScore = Score;
 		//UE_LOG(LogTemp, Warning, TEXT("HUD components are invalid in SetHUDScore"));
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
 		if (HasAuthority())
 		{
 			UE_LOG(LogTemp, Warning, TEXT("HUD components are invalid in SetHUDWeaponAmmo Server"));
 		}
 		//UE_LOG(LogTemp, Warning, TEXT("HUD components are invalid in SetHUDWeaponAmmo"));
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
 		//UE_LOG(LogTemp, Warning, TEXT("CountdownTime: %f"), CountdownTime);
 		if (CountdownTime < 0.f)
 		{
 			MainHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString("Alma"));
 			return;
 		}
 		
 		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
 		int32 Seconds = CountdownTime - Minutes * 60;
 
 		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
 		MainHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));

 	}
 	else
 	{
 		//UE_LOG(LogTemp, Warning, TEXT("HUD components are invalid in SetHUDMatchCountdown"));
 	}
 }


void AMyPlayerController::SetHUDTime()
 {
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
 	if (CountdownInt != SecondsLeft)
 	{
 		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
 		{
 			SetHUDAnnouncementCountdown(TimeLeft);
 		}
 		if (MatchState == MatchState::InProgress)
 		{
 			SetHUDMatchCountdown(TimeLeft);
 		}
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
	SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
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

void AMyPlayerController::OnMatchStateSet(FName State)
 {
 	MatchState = State;
 
 	if (MatchState == MatchState::InProgress)
 	{
 		HandleMatchHasStarted();
 	}
 	else if (MatchState == MatchState::Cooldown)
 	{
 		HandleCooldown();
 	}
 }
 
void AMyPlayerController::OnRep_MatchState()
 {
 	if (MatchState == MatchState::InProgress)
 	{
 		HandleMatchHasStarted();
 	}
 	else if (MatchState == MatchState::Cooldown)
 	{
 		HandleCooldown();
 	}
 }

void AMyPlayerController::HandleMatchHasStarted()
{
	UE_LOG(LogTemp, Warning, TEXT("Match has started!"));
	MainHUD = MainHUD == nullptr ? Cast<AMainHUD>(GetHUD()) : MainHUD;
	//MainHUD = Cast<AMainHUD>(GetHUD());
	if (MainHUD)
	{
		if (!MainHUD->CharacterOverlay)
		{
			MainHUD->AddCharacterOverlay();
		}
		if (MainHUD->Announcement)
		{
			MainHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMyPlayerController::ServerCheckMatchState_Implementation()
{
	//AMainGameMode* GameMode = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(this));
	AMainGameMode* GameMode = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();

		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("There is no game Mode"));
	}
}
 
void AMyPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
	if (MainHUD && MatchState == MatchState::WaitingToStart)
	{
		MainHUD->AddAnnouncement();
	}
}

void AMyPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	MainHUD = MainHUD == nullptr ? Cast<AMainHUD>(GetHUD()) : MainHUD;
	bool bHUDValid = MainHUD &&
		MainHUD->Announcement &&
		MainHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			MainHUD->Announcement->WarmupTime->SetText(FText());
			MainHUD->Announcement->AnnouncementText->SetText(FText());
			MainHUD->Announcement->InfoText->SetText(FText());
			return;
		}
		
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
 
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		MainHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AMyPlayerController::HandleCooldown()
{
	UE_LOG(LogTemp, Warning, TEXT("End of match!"));
	MainHUD = MainHUD == nullptr ? Cast<AMainHUD>(GetHUD()) : MainHUD;
	if (MainHUD)
	{
		MainHUD->CharacterOverlay->RemoveFromParent();

		if (!IsValid(MainHUD->Announcement))
		{
			MainHUD->AddAnnouncement();
		}
		
		bool bHUDValid = IsValid(MainHUD->Announcement) && 
			 IsValid(MainHUD->Announcement->AnnouncementText);
 
		if (bHUDValid)
		{
			MainHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In:");
			MainHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));
	
			SetHUDAnnouncementCountdown(CooldownTime);
			
			AMainGameState* MainGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(this));
			AMainPlayerState* MainPlayerState = GetPlayerState<AMainPlayerState>();
			if (MainGameState && MainPlayerState)
			{
				TArray<AMainPlayerState*> TopPlayers = MainGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner.");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == MainPlayerState)
				{
					InfoTextString = FString("You are the winner!");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win:\n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
 
				MainHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
		else
		{
			if (IsValid(MainHUD->Announcement))
			{
				if (IsValid(MainHUD->Announcement->AnnouncementText))
				{
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("EoM: bHUDValid (AnnouncementText) is false!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("EoM: bHUDValid (Announcement) is false!"));
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EoM: MainHUD is null!"));
	}

	AMainCharacter* MainCharacter = Cast<AMainCharacter>(GetPawn());
	if (MainCharacter && MainCharacter->GetCombat())
	{
		MainCharacter->bDisableGameplay = true;
		MainCharacter->GetCombat()->FireButtonPressed(false);
	}
}

void AMyPlayerController::ServerTryCheckMatchState_Implementation()
{
	AMainGameMode* GameMode = Cast<AMainGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode)
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(
			TimerHandle,
			this,
			&AMyPlayerController::ServerTryCheckMatchState,
			0.5f, // Retry after 0.5 seconds
			false
		);
		return;
	}
    
	ServerCheckMatchState();
}


void AMyPlayerController::CheckPing(float DeltaTime)
{
	if (HasAuthority()) return;
	
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? TObjectPtr<APlayerState>(GetPlayerState<APlayerState>()) : PlayerState;
		if (PlayerState)
		{
			//UE_LOG(LogTemp, Warning, TEXT("PlayerState->ExactPing: %f"), PlayerState->ExactPing);
			if (PlayerState->ExactPing > HighPingThreshold) // ping is compressed; it's actually ping / 4
			{
				HighPingWarning();
				ServerReportPingStatus(true);
			}
			else
			{
				StopHighPingWarning();
				ServerReportPingStatus(false);
			}
		}
		HighPingRunningTime = 0.f;
	}
}

void AMyPlayerController::HighPingWarning()
{
	MainHUD = MainHUD == nullptr ? Cast<AMainHUD>(GetHUD()) : MainHUD;
	bool bHUDValid = MainHUD &&
		MainHUD->CharacterOverlay &&
		MainHUD->CharacterOverlay->HighPingText;
	if (bHUDValid)
	{
		MainHUD->CharacterOverlay->HighPingText->SetOpacity(1.f);
	}
}
 
void AMyPlayerController::StopHighPingWarning()
{
	MainHUD = MainHUD == nullptr ? Cast<AMainHUD>(GetHUD()) : MainHUD;
	bool bHUDValid = MainHUD &&
		MainHUD->CharacterOverlay &&
		MainHUD->CharacterOverlay->HighPingText;
	if (bHUDValid)
	{
		MainHUD->CharacterOverlay->HighPingText->SetOpacity(0.f);
	}
}

void AMyPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing, this);
}