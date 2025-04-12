// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatMessageWidget.h"
#include "Components/TextBlock.h"

void UChatMessageWidget::SetMessage(const FString& Sender, const FString& Message)
{
	if (SenderText)
	{
		SenderText->SetText(FText::FromString(Sender + ":"));
	}

	if (MessageText)
	{
		MessageText->SetText(FText::FromString(Message));
	}
}
