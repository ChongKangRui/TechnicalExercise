// Fill out your copyright notice in the Description page of Project Settings.


#include "TE_PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/WeaponComponent.h"
#include "PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "PlayerInputMapping.h"
#include "InputMappingContext.h"
#include "InputAction.h"

void ATE_PlayerController::OnPossess(APawn* pawn)
{
    Super::OnPossess(pawn);

    if (pawn) {
        m_Character = Cast<APlayerCharacter>(pawn);
        LoadPlayerMappingDataset();
        /*Making sure input component was setup properly*/
        SetupInputComponent();
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
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
    if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (const UInputAction* move = PlayerMappingDataset->MoveAction.Get()) {
            enhancedInputComponent->BindAction(move, ETriggerEvent::Triggered, m_Character.Get(), &APlayerCharacter::Move);
        }

        if (const UInputAction* look = PlayerMappingDataset->LookAction.Get()) {
            enhancedInputComponent->BindAction(look, ETriggerEvent::Triggered, m_Character.Get(), &APlayerCharacter::Look);
        }

        if (const UInputAction* sprint = PlayerMappingDataset->SprintAction.Get()) {
            enhancedInputComponent->BindAction(sprint, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::StartSprint);
            enhancedInputComponent->BindAction(sprint, ETriggerEvent::Canceled, m_Character.Get(), &APlayerCharacter::StopSprint);
            enhancedInputComponent->BindAction(sprint, ETriggerEvent::Completed, m_Character.Get(), &APlayerCharacter::StopSprint);
        }


        if (const UInputAction* jump = PlayerMappingDataset->JumpAction.Get()) {
            enhancedInputComponent->BindAction(jump, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::CharacterJump);
            enhancedInputComponent->BindAction(jump, ETriggerEvent::Canceled, m_Character.Get(), &APlayerCharacter::StopJumping);
            enhancedInputComponent->BindAction(jump, ETriggerEvent::Completed, m_Character.Get(), &APlayerCharacter::StopJumping);
        }

        if (const UInputAction* shoot = PlayerMappingDataset->ShootAction.Get()) {
            enhancedInputComponent->BindAction(shoot, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::StartShoot);
            enhancedInputComponent->BindAction(shoot, ETriggerEvent::Canceled, m_Character.Get(), &APlayerCharacter::StopShoot);
            enhancedInputComponent->BindAction(shoot, ETriggerEvent::Completed, m_Character.Get(), &APlayerCharacter::StopShoot);
        }

        if (const UInputAction* aim = PlayerMappingDataset->AimAction.Get()) {
            enhancedInputComponent->BindAction(aim, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::BeginAim);
            enhancedInputComponent->BindAction(aim, ETriggerEvent::Canceled, m_Character.Get(), &APlayerCharacter::EndAim);
            enhancedInputComponent->BindAction(aim, ETriggerEvent::Completed, m_Character.Get(), &APlayerCharacter::EndAim);
        }

        if (const UInputAction* reload = PlayerMappingDataset->ReloadAction.Get()) {
            enhancedInputComponent->BindAction(reload, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::ReloadOrRespawn);
        }

        if (const UInputAction* selectPistol= PlayerMappingDataset->PistolSelectionAction.Get()) {
            enhancedInputComponent->BindAction(selectPistol, ETriggerEvent::Started, m_Character->WeaponComponent.Get(), &UWeaponComponent::SetWeapon, EWeaponType::Pistol);
        }

        if (const UInputAction* selectSMG = PlayerMappingDataset->SMGSelectionAction.Get()) {
            enhancedInputComponent->BindAction(selectSMG, ETriggerEvent::Started, m_Character->WeaponComponent.Get(), &UWeaponComponent::SetWeapon, EWeaponType::SMG);
        }

        if (const UInputAction* selectShotgun= PlayerMappingDataset->ShotgunSelectionAction.Get()) {
            enhancedInputComponent->BindAction(selectShotgun, ETriggerEvent::Started, m_Character->WeaponComponent.Get(), &UWeaponComponent::SetWeapon, EWeaponType::ShotGun);
        }

        if (const UInputAction* scoreboardTrigger = PlayerMappingDataset->ScoreboardTriggerAction.Get()) {
            enhancedInputComponent->BindAction(scoreboardTrigger, ETriggerEvent::Started, m_Character.Get(), &APlayerCharacter::OpenScoreboard);
            enhancedInputComponent->BindAction(scoreboardTrigger, ETriggerEvent::Canceled, m_Character.Get(), &APlayerCharacter::CloseScoreboard);
            enhancedInputComponent->BindAction(scoreboardTrigger, ETriggerEvent::Completed, m_Character.Get(), &APlayerCharacter::CloseScoreboard);

        }

        UE_LOG(LogTemp, Error, TEXT("PC: Bind input action success"));
    }

    //After finish binding, release the memory of PlayerMappingDataset
    PlayerMappingDataset.Reset();
}

void ATE_PlayerController::LoadPlayerMappingDataset()
{
    //Load the player mapping context and input action from the data asset
    if (PlayerMappingDataset.IsPending()) {
        PlayerMappingDataset.LoadSynchronous();
    }

    if (PlayerMappingDataset.IsValid()) {
        if (const UInputMappingContext* mappingContext = PlayerMappingDataset->DefaultMappingContext.Get()) {
            if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(this->GetLocalPlayer())) {
                subsystem->AddMappingContext(mappingContext, 0);
            }
        }
    }

}


