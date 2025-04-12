#include "ChatComponent.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerShooter/HUD/MainHUD.h"
#include "MultiplayerShooter/HUD/ChatMessageWidget.h"
#include "Components/ScrollBox.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "MultiplayerShooter/HUD/Chat.h"

// Sets default values for this component's properties
UChatComponent::UChatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UChatComponent::BeginPlay()
{
	Super::BeginPlay();

	// Reference to MainHUD on BeginPlay
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController)
	{
		MainHUD = Cast<AMainHUD>(PlayerController->GetHUD());
	}
}

void UChatComponent::Server_SendChatMessage_Implementation(const FString& Sender, const FText& Text)
{
	UE_LOG(LogTemp, Warning, TEXT("Server sending text"));

	// Send message to all clients (multicast)
	Multicast_SendChatMessage(Sender, Text);
}

bool UChatComponent::Server_SendChatMessage_Validate(const FString& Sender, const FText& Text)
{
	// Optionally validate message here (e.g., check for forbidden words)
	return true;
}

void UChatComponent::Multicast_SendChatMessage_Implementation(const FString& Sender, const FText& Text)
{
	UE_LOG(LogTemp, Warning, TEXT("Multicast sending message to all clients"));
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		MainHUD = Cast<AMainHUD>(PlayerController->GetHUD());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player Controller in multicast"));
	}

	if (MainHUD)
	{
		AddChatMessageToUI(Sender, Text);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No MainHUD in multicast"));
	}
}

void UChatComponent::AddChatMessageToUI(const FString& Sender, const FText& Text)
{

	UE_LOG(LogTemp, Warning, TEXT("AddCharMessageToUI entered"));

	if (MainHUD && MainHUD->Chat)
	{
		// Create the widget using the PlayerController as the owner
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			UChatMessageWidget* NewMessageWidget = CreateWidget<UChatMessageWidget>(PlayerController, MainHUD->Chat->ChatMessageWidgetClass);
			if (NewMessageWidget)
			{
				// Set the message and add it to the UI
				NewMessageWidget->SetMessage(Sender, Text.ToString());
				MainHUD->Chat->ChatHistoryBox->AddChild(NewMessageWidget);
				MainHUD->Chat->ChatHistoryBox->ScrollToEnd();
				UE_LOG(LogTemp, Warning, TEXT("Message added"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No New Message widget"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Player Controller"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No MainHUD or chat"));
	}
}
