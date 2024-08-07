// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UArrowComponent;

UCLASS()
class TECHNICALEXERCISE_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBase();

	FVector GetTraceStart() const;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> ShootPointComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> DefaultsSceneRoot;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;

	
};
