// Fill out your copyright notice in the Description page of Project Settings.


#include "DamagePickup.h"

#include "MultiplayerShooter/MainCharacter/MainCharacter.h"

void ADamagePickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
 
	AMainCharacter* BlasterCharacter = Cast<AMainCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		UBuffComponent* Buff = BlasterCharacter->GetBuff();
		if (Buff)
		{
			Buff->BuffDamage(BaseDamageBuff, DamageBuffTime);
		}
	}
 
	Destroy();
}
