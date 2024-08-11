// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_EndScene.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class TECHNICALEXERCISE_API UW_EndScene : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetWinnerName(FString name);

protected:
	UFUNCTION()
	void OnRestartButtonClick();

	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WinnerName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartButton;
	
	
};
