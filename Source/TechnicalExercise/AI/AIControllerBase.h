// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIControllerBase.generated.h"


class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class AAICharacterBase;
class ATechnicalExerciseGameMode;

UCLASS()
class TECHNICALEXERCISE_API AAIControllerBase : public AAIController
{
	GENERATED_BODY()
	
public:
	AAIControllerBase();
	const AActor* GetCurrentTarget() const;
	void OnCharacterDeath();
	void RestartLogic();
	void StopLogic();
	void StartBehaviorTree();
	void SetTarget(AActor* Target);

protected:
	UFUNCTION()
	void onPerceptionUpdated(const TArray<AActor*>& updatedActors);
	
	void BeginPlay() override;
	void OnPossess(APawn* PawnToProcess) override;
	void Tick(float DeltaTime) override;

	void ToggleUseControlRotation(bool bUseControlRotation);
	void ClearTarget();

	void Respawn();
protected :
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Hearing> HearConfig;

private:
	TWeakObjectPtr<AActor> m_Target;
	TWeakObjectPtr<UBehaviorTree> m_BT;
	TObjectPtr<AAICharacterBase> m_PossessedEnemy;
	TObjectPtr<ATechnicalExerciseGameMode> m_GameMode;
};
