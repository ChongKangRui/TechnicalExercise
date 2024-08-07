// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TechnicalExerciseCharacter.h"
#include "PlayerCharacter.generated.h"

class ATE_PlayerController;
class UW_PlayerCrosshair;

UCLASS()
class TECHNICALEXERCISE_API APlayerCharacter : public ATechnicalExerciseCharacter
{
	GENERATED_BODY()
	
public:
	APlayerCharacter();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void BeginPlay() override;
protected:
	/*Move Control*/
	void Move(const FInputActionValue& Value);

	/*Look Control*/
	void Look(const FInputActionValue& Value);

	/*Sprint Control*/
	void StartSprint();
	void StopSprint();

	/*Aim Control*/
	void BeginAim();
	void EndAim();
	void AimTick();

	/*Shoot*/
	void Shoot();

	/*Start/Stop Stamina Regen*/
	void StartStaminaRegen();
	void StopStaminaRegen();

	/*Helper function to stop any timer*/
	void StopTimer(FTimerHandle&  Handle);

	/*Interface*/
	float GetStamina_Implementation() const override;
	float GetMaxStamina_Implementation() const override;

	/*Player weapon trace direction is different with normal character, it take from camera instead*/
	FVector GetWeaponTraceDirection() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(BlueprintReadOnly, Category = "Base Attribute")
	float Stamina = 100.0f;

private:
	void InitializeWidget();

private:
	FTimerHandle m_SprintStaminaCostTimer;
	FTimerHandle m_StaminaRegenTimer;
	FTimerHandle m_AimTimer;

	TSubclassOf<UUserWidget> m_CrosshairWidgetClass;
	TWeakObjectPtr<UW_PlayerCrosshair> m_PlayerCrosshair;

	bool m_IsAiming = false;

	/*Use friend for player controller, so it can access all the control functionality to the player character*/
	friend ATE_PlayerController;
};
