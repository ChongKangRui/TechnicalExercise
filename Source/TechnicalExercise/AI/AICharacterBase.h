// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TechnicalExerciseCharacter.h"
#include "AICharacterBase.generated.h"

class AAIControllerBase;
class UWidgetComponent;
UCLASS()
class TECHNICALEXERCISE_API AAICharacterBase : public ATechnicalExerciseCharacter
{
	GENERATED_BODY()
	
public:
	AAICharacterBase();

	void BeginPlay() override;
	void OnCharacterRevive() override;
	void OnCharacterDeath() override;
	void OnCharacterAllowToStart() override;
	void PossessedBy(AController* controller) override;

	/*Reimplement apply damage to make AI switch enemy if needed*/
	void ApplyDamage_Implementation(const float DamageValue, AActor* DamageSource) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UWidgetComponent> WidgetComponent;

private:
	TObjectPtr<AAIControllerBase> m_AIController;
	
};
