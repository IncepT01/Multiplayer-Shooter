// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Chat.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API UChat : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ChatText;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* InputField;
	//virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void NativeConstruct() override;

	// Handles message commit from InputField (if using OnTextCommitted)
	UFUNCTION()
	void OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	// Optional helper function to send message and reset input mode
	void SendChatMessage(const FText& Text);
};
