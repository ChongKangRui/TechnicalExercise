// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTDecorator_IsTargetOutOfSigh.generated.h"

/**
 * 
 */
UCLASS()
class TECHNICALEXERCISE_API UBTDecorator_IsTargetOutOfSigh : public UBTDecorator_Blackboard
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditInstanceOnly)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditInstanceOnly)
	bool ShouldInSight = true;

private:
	bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
	
};
