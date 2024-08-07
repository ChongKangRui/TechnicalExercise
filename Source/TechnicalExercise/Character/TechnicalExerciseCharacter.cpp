// Copyright Epic Games, Inc. All Rights Reserved.

#include "TechnicalExerciseCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "CharacterTrajectoryComponent.h"
#include "WeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ATechnicalExerciseCharacter::ATechnicalExerciseCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	TrajectoryComponent = CreateDefaultSubobject<UCharacterTrajectoryComponent>(TEXT("Character Trajectory Component"));

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon Component"));
}

void ATechnicalExerciseCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

float ATechnicalExerciseCharacter::GetHealth_Implementation() const
{
	return Health;
}

float ATechnicalExerciseCharacter::GetMaxHealth_Implementation() const
{
	return CharacterAttribute.MaxHealth;
}

void ATechnicalExerciseCharacter::ApplyDamage_Implementation(const float DamageValue)
{
	Health = FMath::Clamp(Health - DamageValue, 0, CharacterAttribute.MaxHealth);
	if (Health <= 0)
		OnCharacterDead();

}

void ATechnicalExerciseCharacter::OnCharacterDead()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesSimulatePhysics(true);
}

FVector ATechnicalExerciseCharacter::GetWeaponTraceDirection() const
{
	return GetActorForwardVector();
}


