// Fill out your copyright notice in the Description page of Project Settings.

#include "Chat.h"

#include "ChatMessageWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "GameFramework/PlayerState.h"
#include "MultiplayerShooter/MainCharacter/MainCharacter.h"
#include "MultiplayerShooter/PlayerController/MyPlayerController.h"
#include "MultiplayerShooter/ActorComponents/ChatComponent.h"

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
		FText msg = InputField->GetText();
		UE_LOG(LogTemp, Display, TEXT("Sending message: %s"), *msg.ToString());

		// Reset input focus
		if (APlayerController* PC = GetOwningPlayer())
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}

		if (!msg.IsEmpty())
		{
			// Example sender name — replace this with a real player name if needed
			FString SenderName = GetOwningPlayer()->PlayerState->GetPlayerName();

			// Get the ChatComponent of the PlayerController
			AMyPlayerController* MyPC = Cast<AMyPlayerController>(GetOwningPlayer());
			if (MyPC)
			{
				// Get the controlled pawn (MainCharacter)
				if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(MyPC->GetPawn()))
				{
					// Send message through ChatComponent on the MainCharacter
					if (UChatComponent* ChatComp = MainCharacter->FindComponentByClass<UChatComponent>())
					{
						ChatComp->Server_SendChatMessage(SenderName, msg);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("ChatComponent not found on MainCharacter!"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("MainCharacter not found!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Player Controller problem in Chat.cpp"));
			}
		}

		// Clear the input field after sending the message
		InputField->SetText(FText::GetEmpty());
	}
}



/*
void UChat::OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		// Send message...
		FText msg = InputField->GetText();
		UE_LOG(LogTemp, Display, TEXT("Sending message: %s"), *msg.ToString());

		// Reset input focus
		if (APlayerController* PC = GetOwningPlayer())
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}

		if (!msg.IsEmpty())
		{
			// Example sender name — replace this with a real player name if needed
			FString SenderName = GetOwningPlayer()->PlayerState->GetPlayerName();;

			GetOwningPlayer()->
		}

		InputField->SetText(FText::GetEmpty());
	}
}


void UChat::Multicast_SendChatMessage_Implementation(const FString& Sender, const FText& Text)
{
	UE_LOG(LogTemp, Warning, TEXT("Entered the multicast"))
	if (!ChatMessageWidgetClass || !ChatHistoryBox) return;

	UChatMessageWidget* NewMessageWidget = CreateWidget<UChatMessageWidget>(this, ChatMessageWidgetClass);
	if (NewMessageWidget)
	{
		NewMessageWidget->SetMessage(Sender, Text.ToString());
		ChatHistoryBox->AddChild(NewMessageWidget);
		ChatHistoryBox->ScrollToEnd();
	}
}

// Send the message to the server
void UChat::Server_SendChatMessage_Implementation(const FString& Sender,const FText& Text)
{
	UE_LOG(LogTemp, Warning, TEXT("Server  sending text"))
	Multicast_SendChatMessage(Sender,Text);
}
*/