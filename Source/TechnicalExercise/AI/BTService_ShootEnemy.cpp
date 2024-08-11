// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_ShootEnemy.h"
#include "AI/AIControllerBase.h"
#include "Character/WeaponComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTService_ShootEnemy::UBTService_ShootEnemy()
{
	/*Making sure not all AI using same service instance*/
	bNotifyBecomeRelevant = true;
	bCreateNodeInstance = true;
}

void UBTService_ShootEnemy::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	/*Initialization, after finish initialization, on become relevant should close and start tick task instead*/
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
	const AActor* target = m_AiController->GetCurrentTarget();
	const AActor* aiCharacter = m_AiController->GetPawn();

	if (!target || !aiCharacter) {
		UE_LOG(LogTemp, Error, TEXT("BTS_ShootEnemy: Invalid Target/Possessed AI"))
		return;
	}

	FVector directionToTarget = target->GetActorLocation() - aiCharacter->GetActorLocation();
	FVector directionToTargetNormalize = directionToTarget.GetSafeNormal();

	//Set Yaw Rotation
	float yawRadian = FMath::Atan2(directionToTargetNormalize.Y, directionToTargetNormalize.X);
	float yawAngle = FMath::RadiansToDegrees(yawRadian);

	//Set Pitch Rotation
	float pitchRadian = FMath::Atan2(directionToTargetNormalize.Z, directionToTargetNormalize.Size2D());
	float pitchAngle = FMath::RadiansToDegrees(pitchRadian);

	float yOffset = 0;
	float pOffset = 0;

	yOffset = FMath::RandRange(MinYawRandomOffset, MaxYawRandomOffset);
	pOffset = FMath::RandRange(MinPitchRandomOffset, MaxPitchRandomOffset);

	//Add offset to prevent pitch for being too high or too low
	//Add random division offset to prevent AI being too OP(Overpowered)
	FRotator targetRotation = FRotator(pitchAngle + PitchBaseOffset + pOffset, yawAngle + yOffset, 0.0f);
	FRotator finalRotation = FMath::RInterpTo(m_AiController->GetControlRotation(),
		targetRotation, DeltaSeconds, 200.0f);

	m_AiController->SetControlRotation(finalRotation);

	/*Disable shooting when enemy hide behind the wall*/
	if (!DisableShooting) {
		if (m_WeaponComponent->CanShoot())
			m_WeaponComponent->StartShooting();
		else {
			/*Switch weapon if needed*/
			if (m_WeaponComponent->GetCurrentMaxWepaonAmmunition() <= 0 && m_WeaponComponent->GetCurrentWeaponAmmunition() <= 0) {
				auto list = m_WeaponComponent->DefaultsWeaponList;
				list.Remove(m_WeaponComponent->GetCurrentWeaponType());

				m_WeaponComponent->SetWeapon(list[0]);
			}
		}
	}

}
