// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TechnicalExerciseGameMode.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Character/WeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/W_PlayerCrosshair.h"
#include "UI/W_StartTimer.h"
#include "UI/W_HealthBar.h"

APlayerCharacter::APlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 400.0f; 
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	/*Get the Player Weapon Info(show ammunition) widget class*/
	static ConstructorHelpers::FClassFinder<UUserWidget> crosshairWidgetClassFinder(TEXT("/Game/Blueprint/UI/W_PlayerWeaponInfo"));
	if (crosshairWidgetClassFinder.Succeeded())
	{
		m_CrosshairWidgetClass = crosshairWidgetClassFinder.Class;
	}

	/*Get the scoreboard widget class*/
	static ConstructorHelpers::FClassFinder<UUserWidget> scoreboardWidgetClassFinder(TEXT("/Game/Blueprint/UI/W_Scoreboard"));
	if (scoreboardWidgetClassFinder.Succeeded())
	{
		m_ScoreboardWidgetClass = scoreboardWidgetClassFinder.Class;
	}

	/*Get the timer widget class*/
	static ConstructorHelpers::FClassFinder<UUserWidget> eventTimerWidgetClassFinder(TEXT("/Game/Blueprint/UI/W_StartTimer"));
	if (eventTimerWidgetClassFinder.Succeeded())
	{
		m_EventTimerWidgetClass = eventTimerWidgetClassFinder.Class;
	}

	/*Get Health/Stamina/Revive Prompt widget class*/
	static ConstructorHelpers::FClassFinder<UUserWidget> eventPlayerHealthWidgetClassFinder(TEXT("/Game/Blueprint/UI/W_Player_HealthBar"));
	if (eventPlayerHealthWidgetClassFinder.Succeeded())
	{
		m_PlayerHealthWidgetClass = eventPlayerHealthWidgetClassFinder.Class;
	}


}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitializeWidget();

	if (!m_GameMode) {
		if (ATechnicalExerciseGameMode* gm = GetWorld()->GetAuthGameMode<ATechnicalExerciseGameMode>()) {
			m_GameMode = gm;
		}
	}

	/*Later this will set to false to enable input in Game Mode*/
	m_DisableControl = true;
}

void APlayerCharacter::InitializeWidget()
{
	if (m_CrosshairWidgetClass) {
		m_PlayerCrosshair = CreateWidget<UW_PlayerCrosshair>(GetWorld(), m_CrosshairWidgetClass);
		m_PlayerCrosshair->WeaponComponent = WeaponComponent;
		m_PlayerCrosshair->AddToViewport();
	}

	if (m_PlayerHealthWidgetClass) {
		m_HealthBar = CreateWidget<UW_HealthBar>(GetWorld(), m_PlayerHealthWidgetClass);
		m_HealthBar->AddToViewport();
		m_HealthBar->SetOwner(this);
	}
}

void APlayerCharacter::OnCharacterRevive()
{
	Super::OnCharacterRevive();
	m_DisableControl = false;
	m_HealthBar->ShowReviveText(false);
}

void APlayerCharacter::OnCharacterDeath()
{
	Super::OnCharacterDeath();
	m_DisableControl = true;

	/*Respawn Timer*/
	if (m_EventTimerWidgetClass) {
		if (m_GameMode && !m_GameMode->IsGameEnd()) {

			m_EventTimer = CreateWidget<UW_StartTimer>(GetWorld(), m_EventTimerWidgetClass);
			m_EventTimer->StartTimer("Can Respawn In", 2.0f);
			m_EventTimer->AddToViewport();

			FTimerHandle tempHandle;
			FTimerDelegate tempDelegate;
			tempDelegate.BindWeakLambda(this, [&]()
				{
					m_CanRespawn = true;
					m_HealthBar->ShowReviveText(true);
				});
			GetWorld()->GetTimerManager().SetTimer(tempHandle, tempDelegate, 1.0f, false, 2.0f);

		}
	}
}

void APlayerCharacter::OnCharacterAllowToStart()
{
	m_DisableControl = false;
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (m_DisableControl)
		return;

	FVector2D movementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator rotation = Controller->GetControlRotation();
		const FRotator yawRotation(0, rotation.Yaw, 0);

		// get forward vector
		const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(forwardDirection, movementVector.Y);
		AddMovementInput(rightDirection, movementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (m_DisableControl)
		return;

	FVector2D lookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(lookAxisVector.X);
		AddControllerPitchInput(lookAxisVector.Y);
	}
}

void APlayerCharacter::CharacterJump()
{
	if (m_DisableControl)
		return;

	Jump();
}

void APlayerCharacter::StartSprint()
{
	if (m_DisableControl)
		return;

	if (m_IsAiming)
		return;

	if (WeaponComponent->IsShooting())
		return;

	StopTimer(m_StaminaRegenTimer);
	StopTimer(m_SprintStaminaCostTimer);
	GetCharacterMovement()->MaxWalkSpeed = CharacterAttribute.SprintSpeed;
	UE_LOG(LogTemp, Error, TEXT("Start Sprint"));
	FTimerDelegate tempDelegate;
	tempDelegate.BindWeakLambda(this, [&]()
		{
			if(GetVelocity().Length() > 0)
				Stamina  = FMath::Clamp(Stamina - CharacterAttribute.SprintCostStamina * 0.1f, 0, CharacterAttribute.MaxStamina);

			if (Stamina <= 0) {
				StopSprint();
			}
		});

	GetWorld()->GetTimerManager().SetTimer(m_SprintStaminaCostTimer, tempDelegate, 0.1f, true);
}

void APlayerCharacter::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = CharacterAttribute.WalkSpeed;
	StopTimer(m_SprintStaminaCostTimer);
	StartStaminaRegen();
}

void APlayerCharacter::BeginAim()
{
	if (m_DisableControl)
		return;

	StopSprint();
	StopTimer(m_AimTimer);
	m_IsAiming = true;
	UE_LOG(LogTemp, Error, TEXT("start aim"));

	/*Start the timer tick for aiming*/
	AimTick();
}

void APlayerCharacter::EndAim()
{
	StopTimer(m_AimTimer);
	m_IsAiming = false;

	/*Stop the timer tick for aim*/
	AimTick();
}

void APlayerCharacter::AimTick()
{
	FTimerDelegate tempDelegate;
	tempDelegate.BindWeakLambda(this, [&]()
		{
			/*Set Camera Target FOV based on m_IsAiming*/
			float targetFOV = m_IsAiming ? CharacterAttribute.AimFOV : CharacterAttribute.NormalFOV;
			/*Smooth Target Interp, speed can be change in character attribute*/
			float newFOV = FMath::FInterpTo(GetFollowCamera()->FieldOfView, targetFOV, GetWorld()->GetDeltaSeconds(), CharacterAttribute.AimSpeed);

			// Ensure that the new FOV is within the target range
			GetFollowCamera()->FieldOfView = FMath::Clamp(newFOV, FMath::Min(CharacterAttribute.NormalFOV, CharacterAttribute.AimFOV), FMath::Max(CharacterAttribute.NormalFOV, CharacterAttribute.AimFOV));

			if (FMath::IsNearlyEqual(GetFollowCamera()->FieldOfView, targetFOV, 0.01f)) {
				StopTimer(m_AimTimer);
				GetFollowCamera()->FieldOfView = targetFOV;
			}
		});

	GetWorld()->GetTimerManager().SetTimer(m_AimTimer, tempDelegate, 0.01f, true);
}

void APlayerCharacter::StartShoot()
{
	if (m_DisableControl)
		return;

	WeaponComponent->StartShooting_Loop();
	StopSprint();
}

void APlayerCharacter::StopShoot()
{
	WeaponComponent->StopShooting_Loop();
}

void APlayerCharacter::StartStaminaRegen()
{
	FTimerDelegate tempDelegate;
	tempDelegate.BindWeakLambda(this, [&]()
		{
			Stamina = FMath::Clamp(Stamina + CharacterAttribute.StaminaRefillSpeed * 0.1f, 0, CharacterAttribute.MaxStamina);
			if (Stamina >= 100.0f) {
				StopStaminaRegen();
			}
		});

	GetWorld()->GetTimerManager().SetTimer(m_StaminaRegenTimer, tempDelegate, 0.1f, true);
}

void APlayerCharacter::StopStaminaRegen()
{
	StopTimer(m_StaminaRegenTimer);
}

void APlayerCharacter::ReloadOrRespawn()
{
	if (m_DisableControl && Health <= 0 && m_CanRespawn) {
		if (m_GameMode) {
			m_GameMode->RespawnTarget(this);
			m_CanRespawn = false;
		}
	}
	else {
		/*Reload*/
		WeaponComponent->StartReload();
	}

}

void APlayerCharacter::OpenScoreboard()
{
	if (m_GameMode) {
		if (!m_GameMode->IsGameStart())
			return;
	}

	if (m_ScoreboardWidgetClass) {
		m_Scoreboard = CreateWidget<UUserWidget>(GetWorld(), m_ScoreboardWidgetClass);
		m_Scoreboard->AddToViewport();
	}
}

void APlayerCharacter::CloseScoreboard()
{
	if (m_Scoreboard.IsValid()) {
		m_Scoreboard->RemoveFromParent();
		m_Scoreboard.Reset();
	}
}

void APlayerCharacter::OnBulletHitBind(AActor* actor)
{
	Super::OnBulletHitBind(actor);
	/*Play hit marker*/
	m_PlayerCrosshair->PlayHitReaction();
}

float APlayerCharacter::GetStamina_Implementation() const
{
	return Stamina;
}

float APlayerCharacter::GetMaxStamina_Implementation() const
{
	return CharacterAttribute.MaxStamina;
}

FVector APlayerCharacter::GetWeaponTraceStartLocation() const
{
	return FollowCamera->GetComponentLocation();
}

FVector APlayerCharacter::GetWeaponTraceEndDirection() const
{
	return FollowCamera->GetForwardVector();
}

void APlayerCharacter::StopTimer(FTimerHandle& Handle)
{
	if (GetWorld()->GetTimerManager().IsTimerActive(Handle)) {
		GetWorld()->GetTimerManager().ClearTimer(Handle);
		Handle.Invalidate();
	}
}
