// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_StartTimer.h"
#include "Components/TextBlock.h"

void UW_StartTimer::StartTimer(FString EventName, int Timer)
{
	EventText->SetText(FText::FromString(FString::Printf(TEXT("%s"), *EventName)));

	m_CurrentTimer = Timer;

	TimerText->SetText(FText::FromString(FString::Printf(TEXT("%i"), m_CurrentTimer)));

	FTimerDelegate tempDelegate;
	tempDelegate.BindWeakLambda(this, [&]()
		{
			m_CurrentTimer--;
			TimerText->SetText(FText::FromString(FString::Printf(TEXT("%i"), m_CurrentTimer)));

			if (m_CurrentTimer <= 0) {
				GetWorld()->GetTimerManager().ClearTimer(m_EventTimer);
				m_EventTimer.Invalidate();

				if(ShouldRemoveFromViewportAfterCountdownEnd)
					this->RemoveFromParent();
			}
		});

	GetWorld()->GetTimerManager().SetTimer(m_EventTimer, tempDelegate, 1.0f, true);


}
