// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_EndScene.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UW_EndScene::NativeConstruct()
{
	Super::NativeConstruct();

    if (RestartButton)
    {
        RestartButton->OnClicked.AddDynamic(this, &UW_EndScene::OnRestartButtonClick);
    }
}

void UW_EndScene::SetWinnerName(FString name)
{
    WinnerName->SetText(FText::FromString(FString::Printf(TEXT("Winner Is %s !!!!!"), *name)));
}

void UW_EndScene::OnRestartButtonClick()
{
    UWorld* World = GetWorld();
    if (World)
    {
        FString CurrentLevelName = World->GetMapName();
        CurrentLevelName.RemoveFromStart(World->StreamingLevelsPrefix);

        UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));
    }
}
