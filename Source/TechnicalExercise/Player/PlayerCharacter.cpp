// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/WeaponComponent.h"
#include "Curves/CurveFloat.h"
#include "UI/W_PlayerCrosshair.h"
#include "TechnicalExerciseGameMode.h"

APlayerCharacter::APlayerCharacter()
{
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 400.0f; 
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	static ConstructorHelpers::FClassFinder<UUserWidget> CrosshairWidgetClassFinder(TEXT("/Game/Blueprint/UI/W_PlayerWeaponInfo"));
	if (CrosshairWidgetClassFinder.Succeeded())
	{
		m_CrosshairWidgetClass = CrosshairWidgetClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> ScoreboardWidgetClassFinder(TEXT("/Game/Blueprint/UI/W_Scoreboard"));
	if (ScoreboardWidgetClassFinder.Succeeded())
	{
		m_ScoreboardWidgetClass = ScoreboardWidgetClassFinder.Class;
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
}

void APlayerCharacter::OnCharacterRevive()
{
	Super::OnCharacterRevive();
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
	FTimerDelegate TempDelegate;
	TempDelegate.BindWeakLambda(this, [&]()
		{
			if(GetVelocity().Length() > 0)
				Stamina  = FMath::Clamp(Stamina - CharacterAttribute.SprintCostStamina * 0.1f, 0, CharacterAttribute.MaxStamina);

			if (Stamina <= 0) {
				StopSprint();
			}
		});

	GetWorld()->GetTimerManager().SetTimer(m_SprintStaminaCostTimer, TempDelegate, 0.1f, true);
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
	AimTick();
}

void APlayerCharacter::EndAim()
{
	StopTimer(m_AimTimer);
	m_IsAiming = false;

	AimTick();
}

void APlayerCharacter::AimTick()
{
	FTimerDelegate tempDelegate;
	tempDelegate.BindWeakLambda(this, [&]()
		{
			float targetFOV = m_IsAiming ? CharacterAttribute.AimFOV : CharacterAttribute.NormalFOV;
			float newFOV = FMath::FInterpTo(GetFollowCamera()->FieldOfView, targetFOV, GetWorld()->GetDeltaSeconds(), CharacterAttribute.AimSpeed);

			// Ensure that the new FOV is within the target range
			GetFollowCamera()->FieldOfView = FMath::Clamp(newFOV, FMath::Min(CharacterAttribute.NormalFOV, CharacterAttribute.AimFOV), FMath::Max(CharacterAttribute.NormalFOV, CharacterAttribute.AimFOV));

			//UE_LOG(LogTemp, Error, TEXT("AimTick, FOV %f, Target FOV %f"), GetFollowCamera()->FieldOfView, TargetFOV);

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
	if (m_DisableControl) {
		FTimerDelegate tempDelegate;
		tempDelegate.BindWeakLambda(this, [&]()
		{
			if (m_GameMode) {
				m_GameMode->RespawnTarget(this);
			}
		});
		GetWorld()->GetTimerManager().SetTimer(m_RespawnTimer, tempDelegate, 1.0f, false, 5.0f);
	}
	else {
		WeaponComponent->StartReload();
	}

}

void APlayerCharacter::StopRespawn()
{
	if (m_DisableControl) {
		GetWorld()->GetTimerManager().ClearTimer(m_RespawnTimer);
		m_RespawnTimer.Invalidate();
	}
}

void APlayerCharacter::OpenScoreboard()
{
	if (m_ScoreboardWidgetClass) {
		m_Scoreboard = CreateWidget<UUserWidget>(GetWorld(), m_ScoreboardWidgetClass);
		m_Scoreboard->AddToViewport();
	}
}

void APlayerCharacter::CloseScoreboard()
{
	if (m_Scoreboard.IsValid()) {
		m_Scoreboard->RemoveFromViewport();
		m_Scoreboard.Reset();
	}
}

void APlayerCharacter::InitializeWidget()
{
	if (m_CrosshairWidgetClass) {
		m_PlayerCrosshair = CreateWidget<UW_PlayerCrosshair>(GetWorld(), m_CrosshairWidgetClass);
		m_PlayerCrosshair->WeaponComponent = WeaponComponent;
		m_PlayerCrosshair->AddToViewport();
	}
}

void APlayerCharacter::OnCharacterDeath()
{
	Super::OnCharacterDeath();
	m_DisableControl = true;
}

void APlayerCharacter::OnBulletHitBind(AActor* actor)
{
	Super::OnBulletHitBind(actor);
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
