// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"


UINTERFACE(MinimalAPI)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

class TECHNICALEXERCISE_API IDamageable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	void ApplyDamage(const float DamageValue);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	void RefillHealth();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	float GetStamina() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Damage")
	float GetMaxStamina() const;
	
};
