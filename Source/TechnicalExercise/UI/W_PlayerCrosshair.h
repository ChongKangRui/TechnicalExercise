// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_PlayerCrosshair.generated.h"

class UWeaponComponent;

UCLASS()
class TECHNICALEXERCISE_API UW_PlayerCrosshair : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void PlayHitReaction();

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UWeaponComponent> WeaponComponent;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HitCrosshairReactionAnimation;
};
