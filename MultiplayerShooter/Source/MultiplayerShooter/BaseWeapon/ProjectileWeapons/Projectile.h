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

	/** 
	 * Used with server-side rewind
	 */
 
	bool bUseServerSideRewind = false;
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;
 
	UPROPERTY(EditAnywhere)
	float InitialSpeed = 15000;

	float Damage = 20.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

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
