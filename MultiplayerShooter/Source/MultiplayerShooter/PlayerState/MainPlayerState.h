// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API AMainPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
 
	/**
	* Replication notifies
	*/
	virtual void OnRep_Score() override;
 
	void AddToScore(float ScoreAmount);
private:
	
	UPROPERTY()
	class AMainCharacter* Character;

	UPROPERTY()
	class AMyPlayerController* Controller;
};
