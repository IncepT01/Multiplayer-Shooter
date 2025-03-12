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

	virtual void Destroyed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	//The emitter
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UNiagaraComponent* TracerComponent;

	//The particle itself
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UNiagaraSystem* TracerNiagaraSystem;
	
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ImpactParticles;
 
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
