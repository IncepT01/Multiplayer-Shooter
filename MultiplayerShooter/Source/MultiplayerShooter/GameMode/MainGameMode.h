// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AMainGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AMainCharacter* ElimmedCharacter, class AMyPlayerController* VictimController, AMyPlayerController*
	                              AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;
 
	float LevelStartingTime = 0.f;
 
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
 
private:
	float CountdownTime = 0.f;
};
