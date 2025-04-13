// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenu.generated.h"

class USlider;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API USettingsMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta = (BindWidget))
	USlider* VolumeSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* SensitivitySlider;
	UFUNCTION()
	void OnSensitivityChanged(float Value);
	void NativeConstruct();
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void OnVolumeChanged(float Value);
};
