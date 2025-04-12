// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API UChatMessageWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SenderText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MessageText;

	UFUNCTION(BlueprintCallable)
	void SetMessage(const FString& Sender, const FString& Message);
};
