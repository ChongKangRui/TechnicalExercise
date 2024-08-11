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

/*Character base attribute, can used for data collection such as datatable*/
/*But since both player and enemy shared the same attribute for now, this will be editable in player/enemy blueprint*/
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attribute")
	TArray<TObjectPtr<UAnimMontage>> HitReaction;

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

	/*On Character Dead/Revive/AllowToStart*/
	/*Both player and AI may have their own internal logic to trigger*/
	virtual void OnCharacterDeath();
	virtual void OnCharacterRevive();
	virtual void OnCharacterAllowToStart();

	/*Get Trace Start/Direction, easier for weaponcomponent to get the trace info*/
	virtual FVector GetWeaponTraceStartLocation() const;
	virtual FVector GetWeaponTraceEndDirection() const;

	// Damageble Interface
	float GetHealth_Implementation() const override;
	float GetMaxHealth_Implementation() const override;
	void RefillHealth_Implementation(float HealthToRefill) override;
	void AddPoint_Implementation() override;
	void ApplyDamage_Implementation(const float DamageValue, AActor* DamageSource) override;

	/*Reference Getter*/
	const FCharacterBaseAttribute& GetCharacterAttribute() const;
	UWeaponComponent* GetWeaponComponent() const;

	/*Ragdoll Physic*/
	void StartRagdoll();
	void StopRagdoll();

protected:
	virtual void BeginPlay();
	void PlayRandomHitReaction();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Base Attribute")
	float Health = 100.0f;

	/*This can be import from data table but since now both AI and player shared the same attribute, then It will just export on Blueprint*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Base Attribute")
	FCharacterBaseAttribute CharacterAttribute;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCharacterTrajectoryComponent> TrajectoryComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UWeaponComponent> WeaponComponent;

private:
	FTransform DefaultsMeshTransform;
};

