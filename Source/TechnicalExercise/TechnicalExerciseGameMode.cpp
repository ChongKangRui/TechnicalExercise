// Copyright Epic Games, Inc. All Rights Reserved.

#include "TechnicalExerciseGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "AI/AIControllerBase.h"
#include "Player/PlayerCharacter.h"
#include "Character/TechnicalExerciseCharacter.h"
#include "Character/PlayerStateBase.h"
#include "Character/WeaponComponent.h"
#include "GameFramework/GameStateBase.h"


ATechnicalExerciseGameMode::ATechnicalExerciseGameMode()
{

}

void ATechnicalExerciseGameMode::InitGameState()
{
	Super::InitGameState();
	CopyOfNameList = NameToAssign;
}

void ATechnicalExerciseGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	AssignNameToTarget(NewPlayer);
}

AActor* ATechnicalExerciseGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> spawnlist;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AActor::StaticClass(), "PlayerDefaultsSpawnPoint", spawnlist);

	if (spawnlist.Num() > 0) {
		return spawnlist[0];
	}

	return nullptr;
}

bool ATechnicalExerciseGameMode::CheckIfGameShouldEnd()
{
	AGameStateBase* gameState = GetWorld()->GetGameState<AGameStateBase>();

	if (gameState)
	{
		APlayerState* playerState = *gameState->PlayerArray.FindByPredicate([&](const APlayerState* ps) {
			if (auto psb = Cast<APlayerStateBase>(ps))
			{
				if (psb->GetPoint() >= 4) {
					return true;
				}
			}
			return false;
			}
		);

		if (playerState) {
			m_WinnerActor = playerState->GetOwner();
			return true;
		}
	}
	return false;
}

void ATechnicalExerciseGameMode::AssignNameToTarget(AController* NewPlayer)
{
	if (APlayerState* ps = NewPlayer->PlayerState) {
		if (APlayerStateBase* psb = Cast<APlayerStateBase>(ps)) {
			if (psb->GetName().IsEmpty()) {
				int selectedname = FMath::RandRange(0, CopyOfNameList.Num() - 1);
				psb->SetName(CopyOfNameList[selectedname]);
				CopyOfNameList.RemoveAt(selectedname);

				UE_LOG(LogTemp, Error, TEXT("Assign name to new player"));
			}
		}

	}
}

void ATechnicalExerciseGameMode::RespawnTarget(ATechnicalExerciseCharacter* PlayerToRespawn)
{
	if (!PlayerToRespawn)
		return;

	TArray<AActor*> spawnlist;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), spawnlist);

	int ToSpawn = FMath::RandRange(0, spawnlist.Num() - 1);

	if (ToSpawn < spawnlist.Num()) {
		if (spawnlist[ToSpawn]) {
			PlayerToRespawn->OnCharacterRevive();
			PlayerToRespawn->SetActorLocation(spawnlist[ToSpawn]->GetActorLocation());
			PlayerToRespawn->GetWeaponComponent()->RefillAllAmmunition();
			IDamageable::Execute_RefillHealth(PlayerToRespawn);
		}
	}


}
