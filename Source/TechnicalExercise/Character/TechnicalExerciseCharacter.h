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
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float MaxStamina;

	/*Stamina Refill Speed per seconds*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float StaminaRefillSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float SprintSpeed;

	/*Cost per seconds*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	float SprintCostStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute/Player Only")
	float AimFOV;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute/Player Only")
	float NormalFOV;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute/Player Only")
	float AimSpeed;
	
	FCharacterBaseAttribute(): MaxHealth(100.0f), MaxStamina(100.0f), StaminaRefillSpeed(5.0f), WalkSpeed(500.0f), SprintSpeed(900.0f), 
		SprintCostStamina(15.0f), AimFOV(50.0f), NormalFOV(90.0f), AimSpeed(3.0f)  
	{
	}
};

class UWeaponComponent;

UCLASS(config=Game)
class ATechnicalExerciseCharacter : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	ATechnicalExerciseCharacter();

	UFUNCTION()
	virtual void OnBulletHitBind(AActor* hitActor);
	// Damageble Interface
	float GetHealth_Implementation() const override;
	float GetMaxHealth_Implementation() const override;
	void RefillHealth_Implementation() override;
	void ApplyDamage_Implementation(const float DamageValue) override;

	/*Reference Getter*/
	const FCharacterBaseAttribute& GetCharacterAttribute() const;
	UWeaponComponent* GetWeaponComponent() const;

	/*On Character Dead/Revive*/
	virtual void OnCharacterDeath();
	virtual void OnCharacterRevive();
	/*A Normalized vector for trace direction*/
	virtual FVector GetWeaponTraceStartLocation() const;
	virtual FVector GetWeaponTraceEndDirection() const;

	void StartRagdoll();
	void StopRagdoll();

protected:
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

private:
	FTransform DefaultsMeshTransform;
};

