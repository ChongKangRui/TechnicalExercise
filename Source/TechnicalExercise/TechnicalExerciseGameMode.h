// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TechnicalExerciseGameMode.generated.h"

class ATechnicalExerciseCharacter;
class AAIControllerBase;

UCLASS(minimalapi)
class ATechnicalExerciseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATechnicalExerciseGameMode();
	void AssignNameToTarget(AController* NewPlayer);
	void RespawnTarget(ATechnicalExerciseCharacter* PlayerToRespawn);
	

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FString> NameToAssign = { "Manny", "Jen", "Chicken", "Monkey", "Snake", "Monkey", "Banana", "Apple" };

protected:
	void InitGameState() override;
	void OnPostLogin(AController* NewPlayer) override;

	/*Choose a defaults player start so player won't crash with AI immedially*/
	AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	bool CheckIfGameShouldEnd();
protected:
	TArray<FString> CopyOfNameList;

private:
	AActor* m_WinnerActor;
};



