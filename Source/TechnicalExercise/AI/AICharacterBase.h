// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TechnicalExerciseCharacter.h"
#include "AICharacterBase.generated.h"

/**
 * 
 */
UCLASS()
class TECHNICALEXERCISE_API AAICharacterBase : public ATechnicalExerciseCharacter
{
	GENERATED_BODY()
	
public:
	AAICharacterBase();

	void OnCharacterRevive() override;
	void OnCharacterDeath() override;
	void PossessedBy(AController* controller) override;
	
	
};
