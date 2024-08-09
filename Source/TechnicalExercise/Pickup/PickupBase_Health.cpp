// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupBase_Health.h"
#include "DamageSystem/Damageable.h"

void APickupBase_Health::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		if (OtherActor->GetClass()->ImplementsInterface(UDamageable::StaticClass())) {
			IDamageable::Execute_RefillHealth(OtherActor);
			Destroy();
		}
	}
		
}
