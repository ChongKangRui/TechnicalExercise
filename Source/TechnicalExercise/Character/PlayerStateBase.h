// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStateBase.generated.h"


UCLASS()
class TECHNICALEXERCISE_API APlayerStateBase : public APlayerState
{
	GENERATED_BODY()
	
public:
	void AddPoint();
	void SetName(FString name);
	FString GetName() const;
	int GetPoint() const;

protected:
	FString Name;
	int Point;
	
	
};
