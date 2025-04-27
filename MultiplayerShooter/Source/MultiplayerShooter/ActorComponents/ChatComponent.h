// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ChatComponent.generated.h"

// Forward declaration of UI classes
class UChatMessageWidget;
class UTextBlock;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERSHOOTER_API UChatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Function to send chat messages
	UFUNCTION(Server, Reliable)
	void Server_SendChatMessage(const FString& Sender, const FText& Text);

	// Multicast function to update all clients
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SendChatMessage(const FString& Sender, const FText& Text);

	// Function to add chat message to UI (called locally on clients)
	void AddChatMessageToUI(const FString& Sender, const FText& Text);

	// Your HUD reference (if you want to manipulate the UI directly)
	UPROPERTY(Transient)
	class AMainHUD* MainHUD;
};
