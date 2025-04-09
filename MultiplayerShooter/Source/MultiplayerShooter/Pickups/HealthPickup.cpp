// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthPickup.h"
 #include  "MultiplayerShooter/MainCharacter/MainCharacter.h"
 #include "MultiplayerShooter/ActorComponents/BuffComponent.h"
 #include "NiagaraFunctionLibrary.h"
 #include "NiagaraComponent.h"
 
 AHealthPickup::AHealthPickup()
 {
 	bReplicates = true;
 }
 
void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
 {
 	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
 
 	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
 	if (MainCharacter)
 	{
 		UBuffComponent* Buff = MainCharacter->GetBuff();
 		if (Buff)
 		{
 			//UE_LOG(LogTemp, Warning, TEXT("picked up Buff %s"), *Buff->GetName());
 			Buff->Heal(HealAmount, HealingTime);
 		}
 	}
 
 	Destroy();
 }
