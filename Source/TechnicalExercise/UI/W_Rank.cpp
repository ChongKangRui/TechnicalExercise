// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_Rank.h"
#include "W_Rank.h"
#include "Character/PlayerStateBase.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "Components/VerticalBoxSlot.h"

void UW_Rank::NativeConstruct()
{
	Super::NativeConstruct();
    InitializeArray();

    GenerateText();
}

void UW_Rank::InitializeArray()
{
    AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();

    if (GameState)
    {
        for (APlayerState* PlayerState : GameState->PlayerArray)
        {
            if (auto ps = Cast<APlayerStateBase>(PlayerState))
            {
                m_PlayerStateArray.AddUnique(ps);
            }
        }
    }

    /*Sort order descending based on point*/
    m_PlayerStateArray.Sort([](const APlayerStateBase& A, const APlayerStateBase& B)
    {
        return A.GetPoint() > B.GetPoint();
    });
}

void UW_Rank::GenerateText()
{
    for (APlayerStateBase* ps : m_PlayerStateArray) {
        if (ps) {
            UTextBlock* scoreText = NewObject<UTextBlock>(this);
            if (scoreText) {
                scoreText->SetFont(TextSetting);
                scoreText->SetJustification(ETextJustify::Center);
                FText textToSet;
                if(ps->IsABot())
                    textToSet = FText::FromString(FString::Printf(TEXT("%s                                                          %i"), 
                        *ps->GetName(), ps->GetPoint()));
                else {
                    textToSet = FText::FromString(FString::Printf(TEXT("%s(Player)                                                  %i"), 
                        *ps->GetName(), ps->GetPoint()));
                }

                scoreText->SetText(textToSet);

                UVerticalBoxSlot* scoreSlot = m_ScoreListContainer->AddChildToVerticalBox(scoreText);
                if (scoreSlot)
                {
                    scoreSlot->SetPadding(FMargin(0.0f,50.0f,0.0f,0.0f));
                }
            }
        }
    }
}
