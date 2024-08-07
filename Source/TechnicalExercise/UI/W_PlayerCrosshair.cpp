// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/W_PlayerCrosshair.h"
#include "Character/WeaponComponent.h"
//#include "Animation/WidgetAnimation.h"

void UW_PlayerCrosshair::PlayHitReaction()
{
	if (HitCrosshairReactionAnimation)
		PlayAnimation(HitCrosshairReactionAnimation);
	
}
