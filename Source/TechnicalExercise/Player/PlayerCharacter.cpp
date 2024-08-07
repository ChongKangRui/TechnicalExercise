// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Curves/CurveFloat.h"
#include "UI/W_PlayerCrosshair.h"


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

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Blueprint/UI/W_PlayerWeaponInfo"));
	if (WidgetClassFinder.Succeeded())
	{
		m_CrosshairWidgetClass = WidgetClassFinder.Class;
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitializeWidget();
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

float APlayerCharacter::GetStamina_Implementation() const
{
	return Stamina;
}

float APlayerCharacter::GetMaxStamina_Implementation() const
{
	return CharacterAttribute.MaxStamina;
}

FVector APlayerCharacter::GetWeaponTraceDirection() const
{
	return GetFollowCamera()->GetForwardVector();
}

void APlayerCharacter::InitializeWidget()
{
	if (m_CrosshairWidgetClass) {
		m_PlayerCrosshair = CreateWidget<UW_PlayerCrosshair>(GetWorld(), m_CrosshairWidgetClass);
		m_PlayerCrosshair->WeaponComponent = WeaponComponent;
		m_PlayerCrosshair->AddToViewport();
	}

}

void APlayerCharacter::StartSprint()
{
	if (m_IsAiming)
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
	FTimerDelegate TempDelegate;
	TempDelegate.BindWeakLambda(this, [&]()
		{
			float TargetFOV = m_IsAiming ? CharacterAttribute.AimFOV : CharacterAttribute.NormalFOV;
			float NewFOV = FMath::FInterpTo(GetFollowCamera()->FieldOfView, TargetFOV, GetWorld()->GetDeltaSeconds(), CharacterAttribute.AimSpeed);

			// Ensure that the new FOV is within the target range
			GetFollowCamera()->FieldOfView = FMath::Clamp(NewFOV, FMath::Min(CharacterAttribute.NormalFOV, CharacterAttribute.AimFOV), FMath::Max(CharacterAttribute.NormalFOV, CharacterAttribute.AimFOV));

			//UE_LOG(LogTemp, Error, TEXT("AimTick, FOV %f, Target FOV %f"), GetFollowCamera()->FieldOfView, TargetFOV);

			if (FMath::IsNearlyEqual(GetFollowCamera()->FieldOfView, TargetFOV, 0.01f)) {
				StopTimer(m_AimTimer);
				GetFollowCamera()->FieldOfView = TargetFOV;
			}
		});

	GetWorld()->GetTimerManager().SetTimer(m_AimTimer, TempDelegate, 0.01f, true);
}

void APlayerCharacter::Shoot()
{
}

void APlayerCharacter::StartStaminaRegen()
{
	FTimerDelegate TempDelegate;
	TempDelegate.BindWeakLambda(this, [&]()
		{
			Stamina = FMath::Clamp(Stamina + CharacterAttribute.StaminaRefillSpeed * 0.1f, 0, CharacterAttribute.MaxStamina);
			if (Stamina >= 100.0f) {
				StopStaminaRegen();
			}
		});

	GetWorld()->GetTimerManager().SetTimer(m_StaminaRegenTimer, TempDelegate, 0.1f, true);
}

void APlayerCharacter::StopStaminaRegen()
{
	StopTimer(m_StaminaRegenTimer);
}

void APlayerCharacter::StopTimer(FTimerHandle& Handle)
{
	if (GetWorld()->GetTimerManager().IsTimerActive(Handle)) {
		GetWorld()->GetTimerManager().ClearTimer(Handle);
		Handle.Invalidate();
	}
}
