// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsTargetOutOfSigh.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

bool UBTDecorator_IsTargetOutOfSigh::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    if (const UObject* Target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName)) {

        if (const AActor* TargetActor = Cast<AActor>(Target)) {

            if (OwnerComp.GetAIOwner()) {
               bool Result =  OwnerComp.GetAIOwner()->LineOfSightTo(TargetActor);

               return Result == ShouldInSight;
            }
        }
    }


	return false;
}
