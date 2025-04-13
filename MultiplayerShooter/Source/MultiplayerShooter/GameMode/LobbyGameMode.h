// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	// Called when a player joins the lobby
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Function to travel to the next level
	void TravelToLevel();
	UAudioComponent* GetMusicAudioComponent() const;

	// Function to register input bindings
	void SetupInput();

	UPROPERTY(EditAnywhere)
	USoundCue* BackgroundMusic;
	
};
