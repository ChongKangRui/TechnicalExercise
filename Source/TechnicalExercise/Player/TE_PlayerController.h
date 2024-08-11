// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TE_PlayerController.generated.h"

class UPlayerInputMapping;
class APlayerCharacter;

UCLASS()
class TECHNICALEXERCISE_API ATE_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void OnPossess(APawn* pawn) override;
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TSoftObjectPtr<UPlayerInputMapping> PlayerMappingDataset;

private:
	void LoadPlayerMappingDataset();

private:
	TObjectPtr<APlayerCharacter> m_Character;
	
};
