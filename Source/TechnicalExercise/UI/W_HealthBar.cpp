// Fill out your copyright notice in the Description page of Project Settings.


#include "W_HealthBar.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "DamageSystem/Damageable.h"

void UW_HealthBar::NativeConstruct()
{
	Super::NativeConstruct();
	ShowReviveText(false);
}

void UW_HealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (Owner)
	{
		if (Owner->GetClass()->ImplementsInterface(UDamageable::StaticClass())) {
			UpdateHealthBar(Owner);
			UpdateStaminaBar(Owner);
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Invalid Owner"));
	}

}

void UW_HealthBar::UpdateHealthBar(const APawn* p) {

	if (!HealthProgressBar)
		return;

	float maxHealth = IDamageable::Execute_GetMaxHealth(p);
	float currentHealth = IDamageable::Execute_GetHealth(p);

	HealthProgressBar->SetPercent(currentHealth / maxHealth);;
}

void UW_HealthBar::UpdateStaminaBar(const APawn* p) {
	if (!StaminaProgressBar)
		return;
	float maxStamina = IDamageable::Execute_GetMaxStamina(p);
	float currentStamina = IDamageable::Execute_GetStamina(p);

	StaminaProgressBar->SetPercent(currentStamina/maxStamina);

}

void UW_HealthBar::ShowReviveText(bool show)
{
	if(!show)
		ReviveText->SetVisibility(ESlateVisibility::Hidden);
	else {
		ReviveText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UW_HealthBar::SetOwner(APawn* pawn)
{
	Owner = pawn;
}
