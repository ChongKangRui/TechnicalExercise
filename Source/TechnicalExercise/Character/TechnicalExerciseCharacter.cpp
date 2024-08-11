// Copyright Epic Games, Inc. All Rights Reserved.

#include "TechnicalExerciseCharacter.h"
#include "TechnicalExerciseGameMode.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "CharacterTrajectoryComponent.h"
#include "Character/WeaponComponent.h"
#include "Character/PlayerStateBase.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponComponent.h"
#include "WeaponBase.h"

ATechnicalExerciseCharacter::ATechnicalExerciseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	/*Make sure character only block bullet trace collision*/
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

	TrajectoryComponent = CreateDefaultSubobject<UCharacterTrajectoryComponent>(TEXT("Character Trajectory Component"));

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponentSystem"));
	WeaponComponent->OnBulletHit.AddDynamic(this, &ATechnicalExerciseCharacter::OnBulletHitBind);

	Tags.Add("Damageable");
}

void ATechnicalExerciseCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultsMeshTransform = GetMesh()->GetRelativeTransform();
}

void ATechnicalExerciseCharacter::StartRagdoll()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
}

void ATechnicalExerciseCharacter::StopRagdoll()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesSimulatePhysics(false);

	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GetMesh()->SetRelativeTransform(DefaultsMeshTransform);
}

void ATechnicalExerciseCharacter::PlayRandomHitReaction()
{
	if (CharacterAttribute.HitReaction.Num() <= 0)
		return;

	int hitReactionInt = FMath::RandRange(0, CharacterAttribute.HitReaction.Num() - 1);
	UAnimMontage* hitReaction = CharacterAttribute.HitReaction[hitReactionInt];
	if (hitReaction)
		PlayAnimMontage(hitReaction);
}

float ATechnicalExerciseCharacter::GetHealth_Implementation() const
{
	return Health;
}

float ATechnicalExerciseCharacter::GetMaxHealth_Implementation() const
{
	return CharacterAttribute.MaxHealth;
}

UWeaponComponent* ATechnicalExerciseCharacter::GetWeaponComponent() const
{
	return WeaponComponent;
}

const FCharacterBaseAttribute& ATechnicalExerciseCharacter::GetCharacterAttribute() const
{
	return CharacterAttribute;
}

FVector ATechnicalExerciseCharacter::GetWeaponTraceStartLocation() const
{
	/*Mainly for AI Trace, for player it will be shoot from Camera*/
	if (auto weapon = WeaponComponent->GetWeaponBlueprint())
		return weapon->GetTraceStart();
	else
		return FVector::Zero();
}

FVector ATechnicalExerciseCharacter::GetWeaponTraceEndDirection() const
{
	if (auto weapon = WeaponComponent->GetWeaponBlueprint())
		return weapon->GetActorForwardVector();
	return GetActorForwardVector();
}

void ATechnicalExerciseCharacter::ApplyDamage_Implementation(const float DamageValue, AActor* DamageSource)
{
	Health = FMath::Clamp(Health - DamageValue, 0, CharacterAttribute.MaxHealth);

	if (Health <= 0) {
		IDamageable::Execute_AddPoint(DamageSource);
		OnCharacterDeath();
	}
	else
		PlayRandomHitReaction();
}

void ATechnicalExerciseCharacter::AddPoint_Implementation()
{
	if (GetPlayerState()) {
		if (auto ps = Cast<APlayerStateBase>(GetPlayerState())) {
			ps->AddPoint();

			if (ATechnicalExerciseGameMode* GM = GetWorld()->GetAuthGameMode<ATechnicalExerciseGameMode>()) {
	
				if (ps->GetPoint() >= GM->EndPoint) {
					GM->EndGame(ps->GetName());
				}
			}
		}

	}
}

void ATechnicalExerciseCharacter::RefillHealth_Implementation(float HealthToRefill)
{
	Health += HealthToRefill;
	if (Health > CharacterAttribute.MaxHealth) {
		Health = CharacterAttribute.MaxHealth;
	}
}

void ATechnicalExerciseCharacter::OnBulletHitBind(AActor* actor)
{
}

void ATechnicalExerciseCharacter::OnCharacterDeath()
{
	StartRagdoll();
}

void ATechnicalExerciseCharacter::OnCharacterRevive()
{
	StopRagdoll();
}

void ATechnicalExerciseCharacter::OnCharacterAllowToStart()
{
}



