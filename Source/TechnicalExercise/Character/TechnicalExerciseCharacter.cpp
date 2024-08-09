// Copyright Epic Games, Inc. All Rights Reserved.

#include "TechnicalExerciseCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "CharacterTrajectoryComponent.h"
#include "WeaponComponent.h"
#include "WeaponBase.h"
#include "Character/WeaponComponent.h"
#include "Character/PlayerStateBase.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

	TrajectoryComponent = CreateDefaultSubobject<UCharacterTrajectoryComponent>(TEXT("Character Trajectory Component"));

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponentSystem"));
	WeaponComponent->OnBulletHit.AddDynamic(this, &ATechnicalExerciseCharacter::OnBulletHitBind);

	Tags.Add("Damageable");

	CharacterAttribute = FCharacterBaseAttribute();
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

float ATechnicalExerciseCharacter::GetHealth_Implementation() const
{
	return Health;
}

float ATechnicalExerciseCharacter::GetMaxHealth_Implementation() const
{
	return CharacterAttribute.MaxHealth;
}

void ATechnicalExerciseCharacter::OnBulletHitBind(AActor* actor)
{
	if (!actor) {
		return;
	}

	if (!actor->GetClass()->ImplementsInterface(UDamageable::StaticClass())) {
		return;
	}

	if (IDamageable::Execute_GetHealth(actor) <= 0) {
		if (GetPlayerState()) {
			if (auto ps = Cast<APlayerStateBase>(GetPlayerState())) {
				ps->AddPoint();
			}
		}

	}
}

void ATechnicalExerciseCharacter::ApplyDamage_Implementation(const float DamageValue)
{
	Health = FMath::Clamp(Health - DamageValue, 0, CharacterAttribute.MaxHealth);
	if (Health <= 0)
		OnCharacterDeath();

}

void ATechnicalExerciseCharacter::RefillHealth_Implementation()
{
	Health = CharacterAttribute.MaxHealth;
}

void ATechnicalExerciseCharacter::OnCharacterDeath()
{
	StartRagdoll();
}

UWeaponComponent* ATechnicalExerciseCharacter::GetWeaponComponent() const
{
	return WeaponComponent;
}

void ATechnicalExerciseCharacter::OnCharacterRevive()
{
	StopRagdoll();
}

const FCharacterBaseAttribute& ATechnicalExerciseCharacter::GetCharacterAttribute() const
{
	return CharacterAttribute;
}

FVector ATechnicalExerciseCharacter::GetWeaponTraceStartLocation() const
{
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


