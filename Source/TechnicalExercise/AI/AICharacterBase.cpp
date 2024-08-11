// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacterBase.h"
#include "AI/AIControllerbase.h"
#include "Character/WeaponComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/W_HealthBar.h"

AAICharacterBase::AAICharacterBase()
{
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationPitch = false;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<UUserWidget> healthWidgetClassFinder(TEXT("/Game/Blueprint/UI/W_AIHealth"));
	if (healthWidgetClassFinder.Succeeded())
	{
		WidgetComponent->SetWidgetClass(healthWidgetClassFinder.Class);
	}

}

void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();
	/*Setup the widget component location properly*/
	if (auto healthWidget = Cast<UW_HealthBar>(WidgetComponent->GetUserWidgetObject())) {
		healthWidget->SetOwner(this);
	}
}

void AAICharacterBase::OnCharacterRevive()
{
	Super::OnCharacterRevive();
	if (m_AIController) {
		m_AIController->RestartLogic();
	}
}

void AAICharacterBase::OnCharacterDeath()
{
	Super::OnCharacterDeath();
	m_AIController->OnCharacterDeath();
}

void AAICharacterBase::PossessedBy(AController* controller)
{
	m_AIController = Cast<AAIControllerBase>(controller);
	Super::PossessedBy(controller);
}

void AAICharacterBase::OnCharacterAllowToStart()
{
	m_AIController->StartBehaviorTree();
}

void AAICharacterBase::ApplyDamage_Implementation(const float DamageValue, AActor* DamageSource)
{
	Health = FMath::Clamp(Health - DamageValue, 0, CharacterAttribute.MaxHealth);

	if (Health <= 0) {
		IDamageable::Execute_AddPoint(DamageSource);
		OnCharacterDeath();
		return;
	}
	else
		PlayRandomHitReaction();

	if (!DamageSource)
		return;

	if (!m_AIController->GetCurrentTarget()) {
		m_AIController->SetTarget(DamageSource);
	}
	else {
		float DistanceToCurrentTarget = FVector::Dist(m_AIController->GetCurrentTarget()->GetActorLocation(), GetActorLocation());
		float DistanceToDamageSource = FVector::Dist(DamageSource->GetActorLocation(), GetActorLocation());
		if (DistanceToDamageSource < DistanceToCurrentTarget)
			m_AIController->SetTarget(DamageSource);
	}

}
