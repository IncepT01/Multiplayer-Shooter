// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenu.h"

#include "Components/Slider.h"
#include "MultiplayerShooter/Persistence/SettingsSaveGame.h"
#include "MultiplayerShooter/PlayerController/MyPlayerController.h"


void USettingsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (VolumeSlider)
	{
		VolumeSlider->OnValueChanged.AddDynamic(this, &USettingsMenu::OnVolumeChanged);

		if (APlayerController* PC = GetOwningPlayer())
		{
			if (AMyPlayerController* MPC = Cast<AMyPlayerController>(PC))
			{
				VolumeSlider->SetValue(MPC->SaveGameObject->SavedVolume); // assuming SavedVolume is a float property in your controller
			}
		}
	}

	if (SensitivitySlider)
	{
		SensitivitySlider->OnValueChanged.AddDynamic(this, &USettingsMenu::OnSensitivityChanged);
	}
}

FReply USettingsMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::I)
	{
		// Call back to controller to close menu
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (AMyPlayerController* MPC = Cast<AMyPlayerController>(PC))
			{
				MPC->ToggleSettingsMenu();
			}
		}
		return FReply::Handled();
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void USettingsMenu::OnVolumeChanged(float Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Volume slider changed: %f"), Value);

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMyPlayerController* MPC = Cast<AMyPlayerController>(PC))
		{
			MPC->SetVolume(Value); // Call your function that applies volume
		}
	}
}

void USettingsMenu::OnSensitivityChanged(float Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Sensitivity slider changed: %f"), Value);
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AMyPlayerController* MPC = Cast<AMyPlayerController>(PC))
		{
			MPC->SetMouseSensitivity(Value);
		}
	}
}

