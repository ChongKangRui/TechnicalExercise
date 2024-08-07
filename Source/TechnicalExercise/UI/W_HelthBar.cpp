// Fill out your copyright notice in the Description page of Project Settings.


#include "W_HelthBar.h"
#include "Components/ProgressBar.h"
#include "DamageSystem/Damageable.h"

void UW_HelthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Owner)
	{
		UpdateHealthBar(Owner);
		UpdateStaminaBar(Owner);
	}
}

void UW_HelthBar::UpdateHealthBar(const APawn* p) {

	if (!HealthProgressBar)
		return;

	float MaxHealth = IDamageable::Execute_GetMaxHealth(p);
	float CurrentHealth = IDamageable::Execute_GetHealth(p);

	HealthProgressBar->SetPercent(CurrentHealth / MaxHealth);;
}

void UW_HelthBar::UpdateStaminaBar(const APawn* p) {
	if (!StaminaProgressBar)
		return;
	float MaxStamina = IDamageable::Execute_GetMaxStamina(p);
	float CurrentStamina = IDamageable::Execute_GetStamina(p);

	StaminaProgressBar->SetPercent(CurrentStamina/MaxStamina);

}
