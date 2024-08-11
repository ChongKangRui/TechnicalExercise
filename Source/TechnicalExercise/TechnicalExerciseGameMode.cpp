// Copyright Epic Games, Inc. All Rights Reserved.

#include "TechnicalExerciseGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/AIControllerBase.h"
#include "Player/PlayerCharacter.h"
#include "Player/TE_PlayerController.h"
#include "Character/TechnicalExerciseCharacter.h"
#include "Character/PlayerStateBase.h"
#include "Character/WeaponComponent.h"
#include "UI/W_StartTimer.h"
#include "UI/W_EndScene.h"


ATechnicalExerciseGameMode::ATechnicalExerciseGameMode()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> eventTimerWidgetClassFinder(TEXT("/Game/Blueprint/UI/W_StartTimer"));
	if (eventTimerWidgetClassFinder.Succeeded())
	{
		m_StartGameTimerClass = eventTimerWidgetClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> endSceneWidgetClassFinder(TEXT("/Game/Blueprint/UI/W_EndScene"));
	if (endSceneWidgetClassFinder.Succeeded())
	{
		m_EndSceneClass = endSceneWidgetClassFinder.Class;
	}
}

void ATechnicalExerciseGameMode::InitGameState()
{
	Super::InitGameState();
	
	m_GameStatus = EGameStatus::NoGame;
	StartGame();

}

void ATechnicalExerciseGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	/*add widget to player screen*/
	UW_StartTimer* eventWidget = CreateWidget<UW_StartTimer>(GetWorld(), m_StartGameTimerClass);
	eventWidget->StartTimer("Game Start At", GameStartDelay);
	eventWidget->AddToViewport();

}

AActor* ATechnicalExerciseGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	/*To make sure player spawn in the selected player start instead of random at the beginning*/
	TArray<AActor*> spawnlist;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AActor::StaticClass(), "PlayerDefaultsSpawnPoint", spawnlist);

	if (spawnlist.Num() > 0) {
		return spawnlist[0];
	}

	return nullptr;
}

void ATechnicalExerciseGameMode::AssignNameToTarget(AController* NewPlayer)
{
	if (APlayerState* ps = NewPlayer->PlayerState) {
		if (APlayerStateBase* psb = Cast<APlayerStateBase>(ps)) {
			if (psb->GetName().IsEmpty()) {
				int selectedname = FMath::RandRange(0, CopyOfNameList.Num() - 1);

				if (!CopyOfNameList.IsValidIndex(selectedname))
					return;

				FString newname = CopyOfNameList[selectedname];

				psb->SetName(newname);
				CopyOfNameList.Remove(newname);
			}
		}

	}
}

void ATechnicalExerciseGameMode::RespawnTarget(ATechnicalExerciseCharacter* PlayerToRespawn)
{
	if (IsGameEnd())
		return;

	if (!PlayerToRespawn)
		return;

	TArray<AActor*> spawnlist;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), spawnlist);

	//Get random spawn location
	int toSpawn = FMath::RandRange(0, spawnlist.Num() - 1);

	//Reset all the necesarry character variable
	if (toSpawn < spawnlist.Num()) {
		if (spawnlist[toSpawn]) {
			PlayerToRespawn->OnCharacterRevive();
			PlayerToRespawn->SetActorLocation(spawnlist[toSpawn]->GetActorLocation());
			PlayerToRespawn->GetWeaponComponent()->RefillAllAmmunition();
			IDamageable::Execute_RefillHealth(PlayerToRespawn, 100.0f);
		}
	}

}

void ATechnicalExerciseGameMode::StartGame()
{
	CopyOfNameList = NameToAssign;

	FTimerHandle tempHandle;
	FTimerDelegate tempDelegate;
	tempDelegate.BindWeakLambda(this, [&]()
		{
			TArray<AActor*> characterList;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATechnicalExerciseCharacter::StaticClass(), characterList);

			for (auto character : characterList) {
				if (character) {
					if (ATechnicalExerciseCharacter* player  = Cast<ATechnicalExerciseCharacter>(character)) {
						/*Let the player control character or AI start behavior tree*/
						/*Assign name to them as well*/
						AssignNameToTarget(player->Controller);
						player->OnCharacterAllowToStart();
						m_GameStatus = EGameStatus::GameStart;
					}
				}
			}
		});
	GetWorld()->GetTimerManager().SetTimer(tempHandle, tempDelegate, 1.0f, false, GameStartDelay);

}

void ATechnicalExerciseGameMode::EndGame(FString Name)
{
	m_GameStatus = EGameStatus::GameEnd;

	/*Add end scene widget to player viewport*/
	if (m_EndSceneClass) {
		UW_EndScene* endSceneWidget = CreateWidget<UW_EndScene>(GetWorld(), m_EndSceneClass);
		if (!endSceneWidget) {
			UE_LOG(LogTemp, Error, TEXT("GM: Invalid End Scene Widget"));
			return;
		}

		endSceneWidget->SetWinnerName(Name);
		endSceneWidget->AddToViewport();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("GM: Invalid End Scene Widget Class"));
	}

	TArray<AActor*> characterList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATechnicalExerciseCharacter::StaticClass(), characterList);

	/*Loop through character array so we can stop all character behavior accordingly*/
	for (auto character : characterList) {
		if (character) {
			if (ATechnicalExerciseCharacter* player = Cast<ATechnicalExerciseCharacter>(character)) {
				//Stop Player and AI Behavior
				player->GetCharacterMovement()->StopMovementImmediately();
				if (player->GetPlayerState()->IsABot()) {
					auto controller = Cast<AAIControllerBase>(player->GetController());
					if(controller)
						controller->StopLogic();
				}
				else {
					auto controller = Cast<ATE_PlayerController>(player->GetController());
					if (controller) {
						player->GetWeaponComponent()->StopShooting_Loop();
						controller->SetInputMode(FInputModeUIOnly());
						controller->bShowMouseCursor = true;
						player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
					}
				}
			}
		}
	}
}

bool ATechnicalExerciseGameMode::IsGameEnd() const
{
	return m_GameStatus == EGameStatus::GameEnd;
}

bool ATechnicalExerciseGameMode::IsGameStart() const
{
	return  m_GameStatus == EGameStatus::GameStart;
}
