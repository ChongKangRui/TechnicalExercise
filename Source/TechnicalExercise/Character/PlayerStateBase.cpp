// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerStateBase.h"

void APlayerStateBase::AddPoint()
{
	Point++;
}

void APlayerStateBase::SetName(FString name)
{
	Name = name;
}

FString APlayerStateBase::GetName() const
{
	return Name;
}

int APlayerStateBase::GetPoint() const
{
	return Point;
}
