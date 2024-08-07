// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "DamageSystem/Damageable.h"
#include "TechnicalExerciseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UCharacterTrajectoryComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);


/*Character base attribute, used for data collection*/
USTRUCT(Blueprintable)
struct FCharacterBaseAttribute : public FTableRowBase {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float MaxStamina = 100.0f;

	/*Stamina Refill Speed per seconds*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float StaminaRefillSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float WalkSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float SprintSpeed = 900.0f;

	/*Cost per seconds*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float SprintCostStamina = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute/Player Only")
	float AimFOV = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute/Player Only")
	float NormalFOV = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute/Player Only")
	float AimSpeed = 3.0f;
};

class UWeaponComponent;

UCLASS(config=Game)
class ATechnicalExerciseCharacter : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	ATechnicalExerciseCharacter();

	// Damageble Interface
	float GetHealth_Implementation() const override;
	float GetMaxHealth_Implementation() const override;
	void ApplyDamage_Implementation(const float DamageValue) override;

	virtual void OnCharacterDead();
	/*A Normalized vector for trace direction*/
	virtual FVector GetWeaponTraceDirection() const;

protected:
	// To add mapping context
	virtual void BeginPlay();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Base Attribute")
	float Health = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Base Attribute")
	FCharacterBaseAttribute CharacterAttribute;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCharacterTrajectoryComponent> TrajectoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UWeaponComponent> WeaponComponent;
};

