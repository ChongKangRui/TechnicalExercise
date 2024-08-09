// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/PickupBase.h"
#include "PickupBase_Ammunition.generated.h"

/**
 * 
 */
UCLASS()
class TECHNICALEXERCISE_API APickupBase_Ammunition : public APickupBase
{
	GENERATED_BODY()
	
	
public:
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;
	
};
