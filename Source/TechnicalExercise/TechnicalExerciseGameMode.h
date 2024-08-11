// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TechnicalExerciseGameMode.generated.h"

class ATechnicalExerciseCharacter;
class AAIControllerBase;
class UW_StartTimer;
class UW_EndScene;


UENUM()
enum EGameStatus {
	NoGame,
	GameStart,
	GameEnd
};

UCLASS(minimalapi)
class ATechnicalExerciseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATechnicalExerciseGameMode();
	void AssignNameToTarget(AController* NewPlayer);
	void RespawnTarget(ATechnicalExerciseCharacter* PlayerToRespawn);
	
	void StartGame();
	void EndGame(FString WinnerName);

	bool IsGameEnd() const;
	bool IsGameStart() const;

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FString> NameToAssign = { "Manny", "Jen", "Chicken", "Monkey", "Snake", "Monkey", "Banana", "Apple" };

	UPROPERTY(EditDefaultsOnly)
	int EndPoint = 5;

	UPROPERTY(EditDefaultsOnly)
	float GameStartDelay = 10.0f;

protected:
	void InitGameState() override;
	void OnPostLogin(AController* NewPlayer) override;

	/*Choose a defaults player start so player won't crash with AI immedially*/
	AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	TArray<FString> CopyOfNameList;

private:
	TEnumAsByte<EGameStatus> m_GameStatus;
	TSubclassOf<UW_StartTimer> m_StartGameTimerClass;
	TSubclassOf<UW_EndScene> m_EndSceneClass;
};



