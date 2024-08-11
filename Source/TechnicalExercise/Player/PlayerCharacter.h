// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TechnicalExerciseCharacter.h"
#include "PlayerCharacter.generated.h"

class ATE_PlayerController;
class UW_PlayerCrosshair;
class ATechnicalExerciseGameMode;
class UW_StartTimer;
class UW_HealthBar;

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
	void OnCharacterRevive() override;
	void OnCharacterAllowToStart() override;
protected:

	/*Player weapon trace direction is different with normal character, it take from camera instead*/
	FVector GetWeaponTraceStartLocation() const override;
	FVector GetWeaponTraceEndDirection() const override;

	void OnCharacterDeath() override;

	/*Interface*/
	float GetStamina_Implementation() const override;
	float GetMaxStamina_Implementation() const override;

	/*Move Control*/
	void Move(const FInputActionValue& Value);

	/*Look Control*/
	void Look(const FInputActionValue& Value);

	/*Jump*/
	void CharacterJump();

	/*Sprint Control*/
	void StartSprint();
	void StopSprint();

	/*Aim Control*/
	void BeginAim();
	void EndAim();
	void AimTick();

	/*Shoot*/
	void StartShoot();
	void StopShoot();

	/*Start/Stop Stamina Regen*/
	void StartStaminaRegen();
	void StopStaminaRegen();

	/*Reload or respawn input*/
	void ReloadOrRespawn();

	/*Open/Close Scoreboard*/
	void OpenScoreboard();
	void CloseScoreboard();

	/*Helper function to stop any timer*/
	void StopTimer(FTimerHandle&  Handle);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(BlueprintReadOnly, Category = "Base Attribute")
	float Stamina = 100.0f;

private:
	void OnBulletHitBind(AActor* actor) override;
	void InitializeWidget();

private:
	FTimerHandle m_SprintStaminaCostTimer;
	FTimerHandle m_StaminaRegenTimer;
	FTimerHandle m_AimTimer;
	
	TSubclassOf<UUserWidget> m_CrosshairWidgetClass;
	TSubclassOf<UUserWidget> m_ScoreboardWidgetClass;
	TSubclassOf<UUserWidget> m_EventTimerWidgetClass;
	TSubclassOf<UUserWidget> m_PlayerHealthWidgetClass;

	TWeakObjectPtr<UW_PlayerCrosshair> m_PlayerCrosshair;
	TWeakObjectPtr<UUserWidget> m_Scoreboard;
	TWeakObjectPtr<UW_StartTimer> m_EventTimer;
	TWeakObjectPtr<UW_HealthBar> m_HealthBar;

	TObjectPtr<ATechnicalExerciseGameMode> m_GameMode;

	bool m_IsAiming = false;
	bool m_DisableControl = false;
	bool m_CanRespawn = false;
	/*Use friend for player controller, so it can access all the control functionality to the player character*/
	friend ATE_PlayerController;
};
