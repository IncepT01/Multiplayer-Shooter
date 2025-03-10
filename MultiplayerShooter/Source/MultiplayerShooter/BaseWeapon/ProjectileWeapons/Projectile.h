// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class MULTIPLAYERSHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UNiagaraComponent* TracerComponent;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UNiagaraSystem* TracerNiagaraSystem;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
