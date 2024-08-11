// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsTargetOutOfSigh.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"


UBTDecorator_IsTargetOutOfSigh::UBTDecorator_IsTargetOutOfSigh()
{
    bCreateNodeInstance = true;
    bAllowAbortChildNodes = true;

}

bool UBTDecorator_IsTargetOutOfSigh::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    if (const UObject* target = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName)) {

        if (const AActor* targetActor = Cast<AActor>(target)) {

            if (OwnerComp.GetAIOwner()) {
               bool result =  OwnerComp.GetAIOwner()->LineOfSightTo(targetActor, FVector(0), true);
               //Switch and abort task based on the result
               return result == ShouldInSight;
            }
        }
    }
    return true;
}



