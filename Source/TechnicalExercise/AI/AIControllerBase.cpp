// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AICharacterBase.h"
#include "DamageSystem/Damageable.h"
#include "TechnicalExerciseGameMode.h"

AAIControllerBase::AAIControllerBase()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	HearConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	// Setup perception configuration
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->ConfigureSense(*HearConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());

	/*Make sure AI Detect everyone since everyone is enemy*/
	FAISenseAffiliationFilter detection;
	detection.bDetectEnemies = true;
	detection.bDetectFriendlies = true;
	detection.bDetectNeutrals = true;

	SightConfig->SightRadius = 2500;
	SightConfig->LoseSightRadius = 3000;
	SightConfig->PeripheralVisionAngleDegrees = 75.0f;
	SightConfig->DetectionByAffiliation = detection;

	HearConfig->HearingRange = 4000;
	HearConfig->DetectionByAffiliation = detection;

	bWantsPlayerState = true;

	/*Higher Tick Interval for optimization purpose*/
	SetActorTickInterval(0.5f);
	SetActorTickEnabled(false);

	/*Get Behavior Tree*/
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Script/AIModule.BehaviorTree'/Game/Blueprint/AI/BehaviorTree/BT_AI.BT_AI'"));
	if (BTObject.Succeeded())
	{
		m_BT = BTObject.Object;
	}
}

void AAIControllerBase::onPerceptionUpdated(const TArray<AActor*>& updatedActors)
{
	if (!GetBlackboardComponent()) {
		UE_LOG(LogTemp, Error, TEXT("%s: Invalid Blackboard Component"), *GetName())
			return;
	}

	if (m_Target.IsValid())
		return;

	for (AActor* target : updatedActors) {
		if (target == m_PossessedEnemy)
			continue;

		/*If target alive, set it as AI Target*/
		if (IDamageable::Execute_GetHealth(target) > 0) {
			SetTarget(target);
			SetActorTickEnabled(true);
		}
	}
}

void AAIControllerBase::OnPossess(APawn* PawnToProcess)
{
	Super::OnPossess(PawnToProcess);

	/*Initialize Enemy Character reference*/
	if (AAICharacterBase* enemy = Cast<AAICharacterBase>(PawnToProcess)) {
		m_PossessedEnemy = enemy;
	}
	/*Bind event to OnPErceptionUpdated*/
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIControllerBase::onPerceptionUpdated);
}

void AAIControllerBase::BeginPlay()
{
	Super::BeginPlay();
	/*Initialize Game Mode Reference*/
	if (!m_GameMode) {
		if (ATechnicalExerciseGameMode* gm = GetWorld()->GetAuthGameMode<ATechnicalExerciseGameMode>()) {
			m_GameMode = gm;
		}
	}
}

void AAIControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*Check if target death*/
	if (m_Target.IsValid()) {
		if (IDamageable::Execute_GetHealth(m_Target.Get()) <= 0) {
			ClearTarget();
			SetActorTickEnabled(false);

		}
	}
}

void AAIControllerBase::OnCharacterDeath()
{
	StopLogic();
	Respawn();
}

void AAIControllerBase::RestartLogic()
{
	ClearTarget();
	BrainComponent->RestartLogic();
}

void AAIControllerBase::StopLogic()
{
	SetActorTickEnabled(false);
	ClearTarget();

	BrainComponent->StopLogic("Death/EndGame");
}

void AAIControllerBase::StartBehaviorTree()
{
	if (m_BT.IsValid())
		RunBehaviorTree(m_BT.Get());

	ClearTarget();
}

void AAIControllerBase::ToggleUseControlRotation(bool bUseControlRotation)
{
	if (m_PossessedEnemy) {
		m_PossessedEnemy->GetCharacterMovement()->bOrientRotationToMovement = !bUseControlRotation;
		m_PossessedEnemy->GetCharacterMovement()->bUseControllerDesiredRotation = bUseControlRotation;
	}
}

void AAIControllerBase::SetTarget(AActor* Target)
{
	if (!Target)
		return;

	m_Target.Reset();

	m_Target = Target;
	GetBlackboardComponent()->SetValueAsObject("Target", m_Target.Get());
	ToggleUseControlRotation(true);

	if (m_PossessedEnemy) {
		m_PossessedEnemy->GetCharacterMovement()->MaxWalkSpeed = m_PossessedEnemy->GetCharacterAttribute().WalkSpeed;
	}
}

void AAIControllerBase::ClearTarget()
{
	if (GetBlackboardComponent()) {
		GetBlackboardComponent()->ClearValue("Target");
		m_Target.Reset();
		ToggleUseControlRotation(false);
	}

	if(m_PossessedEnemy)
		m_PossessedEnemy->GetCharacterMovement()->MaxWalkSpeed = m_PossessedEnemy->GetCharacterAttribute().SprintSpeed;
}

void AAIControllerBase::Respawn()
{
	FTimerHandle tempHandle;
	FTimerDelegate tempDelegate;
	
	tempDelegate.BindWeakLambda(this, [&, this]()
		{
			if (m_GameMode) {
				m_GameMode->RespawnTarget(m_PossessedEnemy.Get());
			}
		});
	GetWorld()->GetTimerManager().SetTimer(tempHandle, tempDelegate,1.0f, false, 5.0f);
}

const AActor* AAIControllerBase::GetCurrentTarget() const
{
	return m_Target.Get();
}
