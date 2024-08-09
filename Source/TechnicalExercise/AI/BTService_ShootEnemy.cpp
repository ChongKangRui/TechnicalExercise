// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_ShootEnemy.h"
#include "Character/WeaponComponent.h"
#include "AI/AIControllerBase.h"

#include "BehaviorTree/BehaviorTreeComponent.h"

UBTService_ShootEnemy::UBTService_ShootEnemy()
{
	bNotifyBecomeRelevant = true;
	bCreateNodeInstance = true;
}

void UBTService_ShootEnemy::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner()) {
		if (AAIControllerBase* controller = Cast<AAIControllerBase>(OwnerComp.GetAIOwner())) {
			m_AiController = controller;

			if (controller->GetPawn())
				m_WeaponComponent = controller->GetPawn()->GetComponentByClass<UWeaponComponent>();

		}
	}

	bNotifyTick = true;
	bNotifyBecomeRelevant = false;
}

void UBTService_ShootEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!m_AiController.IsValid() || !m_WeaponComponent.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("BTS_ShootEnemy: Invalid AIController or WeaponComponent, can't perform shoot action"))
		return;
	}
	const AActor* Target = m_AiController->GetCurrentTarget();
	const AActor* AICharacter = m_AiController->GetPawn();

	if (!Target || !AICharacter) {
		UE_LOG(LogTemp, Error, TEXT("BTS_ShootEnemy: Invalid Target/Possessed AI"))
		return;
	}

	FVector DirectionToTarget = Target->GetActorLocation() - AICharacter->GetActorLocation();
	FVector DirectionToTargetNormalize = DirectionToTarget.GetSafeNormal();

	//Set Yaw Rotation
	float YawRadian = FMath::Atan2(DirectionToTargetNormalize.Y, DirectionToTargetNormalize.X);
	float YawAngle = FMath::RadiansToDegrees(YawRadian);

	//Set Pitch Rotation
	float PitchRadian = FMath::Atan2(DirectionToTargetNormalize.Z, DirectionToTargetNormalize.Size2D());
	float PitchAngle = FMath::RadiansToDegrees(PitchRadian);

	float yOffset = 0;
	float pOffset = 0;

	yOffset = FMath::RandRange(MinYawRandomOffset, MaxYawRandomOffset);
	pOffset = FMath::RandRange(MinPitchRandomOffset, MaxPitchRandomOffset);
	

	FRotator TargetRotation = FRotator(PitchAngle + pOffset, YawAngle + yOffset, 0.0f);
	FRotator FinalRotation = FMath::RInterpTo(m_AiController->GetControlRotation(),
		TargetRotation, DeltaSeconds, 100.0f);

	m_AiController->SetControlRotation(FinalRotation);

	if (!DisableShooting) {
		if (m_WeaponComponent->CanShoot())
			m_WeaponComponent->StartShooting();
	}

}
