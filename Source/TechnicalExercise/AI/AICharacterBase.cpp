// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacterBase.h"
#include "AI/AIControllerbase.h"
#include "Character/WeaponComponent.h"


AAICharacterBase::AAICharacterBase()
{
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;
}

void AAICharacterBase::OnCharacterRevive()
{
	Super::OnCharacterRevive();
	if (auto AIController = Cast<AAIControllerBase>(GetController())) {
		AIController->RestartLogic();
	}
}

void AAICharacterBase::OnCharacterDeath()
{
	Super::OnCharacterDeath();

	if (auto ai = Cast<AAIControllerBase>(GetController())) {
		ai->OnCharacterDeath();
	}
}

void AAICharacterBase::PossessedBy(AController* controller)
{
	Super::PossessedBy(controller);
}
