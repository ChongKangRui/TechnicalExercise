// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TE_PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/WeaponComponent.h"
#include "PlayerCharacter.h"
#include "PlayerInputMapping.h"
#include "InputMappingContext.h"
#include "InputAction.h"

void ATE_PlayerController::OnPossess(APawn* pawn)
{
    Super::OnPossess(pawn);

    if (pawn) {
        m_Character = Cast<APlayerCharacter>(pawn);
        loadPlayerMappingDataset();
        SetupInputComponent();
    }
}

void ATE_PlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!PlayerMappingDataset.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("PC: PlayerMappingData Reference Invalid"));
        return;
    }

    if (!m_Character) {
        UE_LOG(LogTemp, Error, TEXT("PC: Invalid Character Reference"));
        return;
    }
    //Binding Input Action
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (const UInputAction* Move = PlayerMappingDataset->MoveAction.Get()) {
            EnhancedInputComponent->BindAction(Move, ETriggerEvent::Triggered, m_Character.Get(), &APlayerCharacter::Move);
        }

        if (const UInputAction* Look = PlayerMappingDataset->LookAction.Get()) {
           EnhancedInputComponent->BindAction(Look, ETriggerEvent::Triggered, m_Character.Get(), &APlayerCharacter::Look);
        }

        if (const UInputAction* Sprint = PlayerMappingDataset->SprintAction.Get()) {
            EnhancedInputComponent->BindAction(Sprint, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::StartSprint);
            EnhancedInputComponent->BindAction(Sprint, ETriggerEvent::Canceled, m_Character.Get(), &APlayerCharacter::StopSprint);
            EnhancedInputComponent->BindAction(Sprint, ETriggerEvent::Completed, m_Character.Get(), &APlayerCharacter::StopSprint);
        }


        if (const UInputAction* Jump = PlayerMappingDataset->JumpAction.Get()) {
            EnhancedInputComponent->BindAction(Jump, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::Jump);
            EnhancedInputComponent->BindAction(Jump, ETriggerEvent::Canceled, m_Character.Get(), &APlayerCharacter::StopJumping);
            EnhancedInputComponent->BindAction(Jump, ETriggerEvent::Completed, m_Character.Get(), &APlayerCharacter::StopJumping);
        }

        if (const UInputAction* Shoot = PlayerMappingDataset->ShootAction.Get()) {
          //  EnhancedInputComponent->BindAction(Shoot, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::meleeAttack);
        }

        if (const UInputAction* Aim = PlayerMappingDataset->AimAction.Get()) {
            EnhancedInputComponent->BindAction(Aim, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::BeginAim);
            EnhancedInputComponent->BindAction(Aim, ETriggerEvent::Canceled, m_Character.Get(), &APlayerCharacter::EndAim);
            EnhancedInputComponent->BindAction(Aim, ETriggerEvent::Completed, m_Character.Get(), &APlayerCharacter::EndAim);
        }

        if (const UInputAction* Reload = PlayerMappingDataset->ReloadAction.Get()) {
            EnhancedInputComponent->BindAction(Reload, ETriggerEvent::Started, m_Character->WeaponComponent.Get(), &UWeaponComponent::ReloadCurrentWeapon);
        }

        if (const UInputAction* SelectPistol= PlayerMappingDataset->PistolSelectionAction.Get()) {
            EnhancedInputComponent->BindAction(SelectPistol, ETriggerEvent::Started, m_Character->WeaponComponent.Get(), &UWeaponComponent::SetWeapon, EWeaponType::Pistol);
        }

        if (const UInputAction* SelectSMG = PlayerMappingDataset->SMGSelectionAction.Get()) {
            EnhancedInputComponent->BindAction(SelectSMG, ETriggerEvent::Started, m_Character->WeaponComponent.Get(), &UWeaponComponent::SetWeapon, EWeaponType::SMG);
        }

        if (const UInputAction* SelectShotgun= PlayerMappingDataset->ShotgunSelectionAction.Get()) {
            EnhancedInputComponent->BindAction(SelectShotgun, ETriggerEvent::Started, m_Character->WeaponComponent.Get(), &UWeaponComponent::SetWeapon, EWeaponType::ShotGun);
        }




        UE_LOG(LogTemp, Error, TEXT("PC: Bind input action success"));
    }

    //After finish binding, release the memory of PlayerMappingDataset
    PlayerMappingDataset.Reset();
}

void ATE_PlayerController::loadPlayerMappingDataset()
{
    //Load the player mapping context and input action from the data asset
    if (PlayerMappingDataset.IsPending()) {
        PlayerMappingDataset.LoadSynchronous();
    }

    if (PlayerMappingDataset.IsValid()) {
        if (const UInputMappingContext* MappingContext = PlayerMappingDataset->DefaultMappingContext.Get()) {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer())) {
                Subsystem->AddMappingContext(MappingContext, 0);
            }
        }
    }

}


