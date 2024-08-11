// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/PickupBase.h"
#include "PickupBase_Health.generated.h"

/**
 * 
 */
UCLASS()
class TECHNICALEXERCISE_API APickupBase_Health : public APickupBase
{
	GENERATED_BODY()
	
protected:
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;
	
	
};
