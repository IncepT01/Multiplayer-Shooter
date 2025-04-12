// Fill out your copyright notice in the Description page of Project Settings.


#include "Chat.h"

#include "Components/EditableTextBox.h"

void UChat::NativeConstruct()
{
	Super::NativeConstruct();

	if (InputField)
	{
		InputField->OnTextCommitted.AddDynamic(this, &UChat::OnChatTextCommitted);
	}
}

void UChat::OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// Send message...

		// Reset input focus
		if (APlayerController* PC = GetOwningPlayer())
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}

		InputField->SetText(FText::GetEmpty());
	}
}