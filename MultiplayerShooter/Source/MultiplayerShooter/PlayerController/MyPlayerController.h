// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
 
 #include "CoreMinimal.h"
 #include "GameFramework/PlayerController.h"
 #include "MyPlayerController.generated.h"
 
 /**
  * 
  */
 UCLASS()
class MULTIPLAYERSHOOTER_API AMyPlayerController : public APlayerController
 {
 	GENERATED_BODY()
 public:
 	void SetHUDHealth(float Health, float MaxHealth);
 protected:
 	virtual void BeginPlay() override;
 	virtual void OnPossess(APawn* InPawn) override;
 
 private:
 	class AMainHUD* MainHUD;
 };
