// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_StartTimer.generated.h"

class UTextBlock;

UCLASS()
class TECHNICALEXERCISE_API UW_StartTimer : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void StartTimer(FString EventName, int Timer);

public:
	bool ShouldRemoveFromViewportAfterCountdownEnd = true;

protected:
	/*Refering to the event that gonna happen like (Respawn In/ Game Start at) etc*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EventText;

	/*The time*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;

private:
	int m_CurrentTimer;
	FTimerHandle m_EventTimer;
	
};
