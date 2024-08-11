// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_ShootEnemy.generated.h"

class UWeaponComponent;
class AAIControllerBase;

UCLASS()
class TECHNICALEXERCISE_API UBTService_ShootEnemy : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTService_ShootEnemy();
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
public:
	UPROPERTY(EditInstanceOnly)
	float MinYawRandomOffset = -30;

	UPROPERTY(EditInstanceOnly)
	float MaxYawRandomOffset = 30;

	UPROPERTY(EditInstanceOnly)
	float MinPitchRandomOffset = -40;

	UPROPERTY(EditInstanceOnly)
	float MaxPitchRandomOffset = 30;

	UPROPERTY(EditInstanceOnly)
	float PitchBaseOffset = -5.0f;

	UPROPERTY(EditInstanceOnly)
	bool DisableShooting = false;

private:
	TWeakObjectPtr<UWeaponComponent> m_WeaponComponent;
	TWeakObjectPtr<AAIControllerBase> m_AiController;
};
