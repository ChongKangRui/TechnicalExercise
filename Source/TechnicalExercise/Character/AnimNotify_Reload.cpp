// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_Reload.h"
#include "WeaponComponent.h"

void UAnimNotify_Reload::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AActor* owner = MeshComp->GetOwner()) {
		if (UWeaponComponent* wc = owner->GetComponentByClass<UWeaponComponent>()) {
			wc->ReloadCurrentWeapon();
		}
	}
}
