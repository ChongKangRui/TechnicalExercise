// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupBase_Ammunition.h"
#include "Character/WeaponComponent.h"

void APickupBase_Ammunition::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CanPickup) {
		if (OtherActor)
			if (auto wp = OtherActor->GetComponentByClass<UWeaponComponent>()) {
				wp->RefillAllAmmunition();
				StartPickupCoolDown();
			}
	}
}
