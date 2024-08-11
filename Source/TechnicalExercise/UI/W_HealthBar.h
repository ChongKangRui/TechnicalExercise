// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_HealthBar.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class TECHNICALEXERCISE_API UW_HealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	/*Revive text Only for player*/
	void ShowReviveText(bool show);
	void SetOwner(APawn* pawn);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateHealthBar(const APawn* p);
	void UpdateStaminaBar(const APawn* p);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReviveText;

	TObjectPtr<APawn> Owner;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaProgressBar;
};
