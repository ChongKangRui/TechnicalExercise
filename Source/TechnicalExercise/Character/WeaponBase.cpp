#include "WeaponBase.h"
#include "Character/WeaponBase.h"
#include "Components/ArrowComponent.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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


