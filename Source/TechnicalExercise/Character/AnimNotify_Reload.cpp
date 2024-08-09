// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_Reload.h"
#include "WeaponComponent.h"

void UAnimNotify_Reload::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AActor* Owner = MeshComp->GetOwner()) {
		if (UWeaponComponent* Wc = Owner->GetComponentByClass<UWeaponComponent>()) {
			Wc->ReloadCurrentWeapon();
		}
	}
}
