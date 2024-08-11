// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQueryContext_Target.h"
#include "AIControllerBase.h"
#include "AICharacterBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_Target::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
    /*QueryCenter need to be the target*/
    if (AAICharacterBase*  queryOwner = Cast<AAICharacterBase>(QueryInstance.Owner.Get())) {
        if (AAIControllerBase* controller = Cast<AAIControllerBase>(queryOwner->GetController())) {

            if (controller->GetCurrentTarget()) {
                UEnvQueryItemType_Actor::SetContextHelper(ContextData, controller->GetCurrentTarget());
            }
            else {
                UEnvQueryItemType_Actor::SetContextHelper(ContextData, queryOwner);
            }
        }
    }

   
}
