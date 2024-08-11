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
	/*Having DefaultsSceneRoot will be easier for designer to gauge and adjust weapon mesh transform*/
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> ShootPointComponent;

	/*The defaults trace start location*/
	/*The tracer vfx will using the location from this ShootPointComponent as well*/
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> DefaultsSceneRoot;

	/*Indicate the forward vector for weapon, easier to adjust the weapon position in blueprint*/
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;

	
};
