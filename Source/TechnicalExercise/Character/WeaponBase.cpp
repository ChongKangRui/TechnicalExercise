#include "WeaponBase.h"
#include "Character/WeaponBase.h"
#include "Components/ArrowComponent.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultsSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultsRoot"));
	DefaultsSceneRoot->SetupAttachment(RootComponent);

	ShootPointComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ShootingStartPoint"));
	ShootPointComponent->SetupAttachment(DefaultsSceneRoot);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(DefaultsSceneRoot);
}

FVector AWeaponBase::GetTraceStart() const
{
	return ShootPointComponent->GetComponentLocation();
}


