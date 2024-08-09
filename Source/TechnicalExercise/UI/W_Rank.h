// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_Rank.generated.h"

class APlayerStateBase;
class UVerticalBox;

UCLASS()
class TECHNICALEXERCISE_API UW_Rank : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void NativeConstruct() override;
	
public:
	UPROPERTY(EditDefaultsOnly)
	FSlateFontInfo TextSetting;

private:
	void InitializeArray();
	void GenerateText();

private:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* m_ScoreListContainer;

	TArray<TObjectPtr<APlayerStateBase>> m_PlayerStateArray;

};
