
#include "Character/WeaponComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "WeaponBase.h"
#include "DamageSystem/Damageable.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "UI/W_PlayerCrosshair.h"
#include "Character/TechnicalExerciseCharacter.h"

/*Helper function for bullet spread and exponent*/
FVector VRandConeNormalDistribution(const FVector& Dir, const float ConeHalfAngleRad, const float Exponent)
{
	if (ConeHalfAngleRad > 0.f)
	{
		const float ConeHalfAngleDegrees = FMath::RadiansToDegrees(ConeHalfAngleRad);

		// consider the cone a concatenation of two rotations. one "away" from the center line, and another "around" the circle
		// apply the exponent to the away-from-center rotation. a larger exponent will cluster points more tightly around the center
		const float FromCenter = FMath::Pow(FMath::FRand(), Exponent);
		const float AngleFromCenter = FromCenter * ConeHalfAngleDegrees;
		const float AngleAround = FMath::FRand() * 360.0f;

		FRotator Rot = Dir.Rotation();
		FQuat DirQuat(Rot);
		FQuat FromCenterQuat(FRotator(0.0f, AngleFromCenter, 0.0f));
		FQuat AroundQuat(FRotator(0.0f, 0.0, AngleAround));
		FQuat FinalDirectionQuat = DirQuat * AroundQuat * FromCenterQuat;

		FinalDirectionQuat.Normalize();

		return FinalDirectionQuat.RotateVector(FVector::ForwardVector) * Dir.Size();
	}
	else
	{
		return Dir;
	}
}

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	DataTableAssetInitialization();
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
		m_Owner = Cast<ATechnicalExerciseCharacter>(GetOwner());

	InitWeaponList();
	SetWeapon(DefaultsWeapon);
}

void UWeaponComponent::InitWeaponList()
{
	for (EWeaponType type : DefaultsWeaponList) {
		m_WeaponList.Add(FStoredWeapon(type, GetWeaponAttributeRow(type)));
	}
}

void UWeaponComponent::DataTableAssetInitialization()
{
	//Get datatable
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTablePath(TEXT("/Script/Engine.DataTable'/Game/DataAsset/DT_Weapon.DT_Weapon'"));
	m_DataTableAsset = DataTablePath.Object;
}

void UWeaponComponent::SetWeapon(const EWeaponType Type)
{
	if (m_Owner.IsValid()) {

		const FStoredWeapon& WeaponData = GetWeapon(Type);
		if (!WeaponData.Attribute.WeaponClass) {
			UE_LOG(LogTemp, Error, TEXT("Invalid Weapon Base Class"));
			return;
		}

		if (m_WeaponBlueprint.IsValid())
			GetWorld()->DestroyActor(m_WeaponBlueprint.Get());

		if(UAnimMontage* m = WeaponData.Attribute.SwitchWeaponMontage)
			m_Owner->PlayAnimMontage(m);

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ActorSpawnParams.Owner = m_Owner.Get();
		m_WeaponBlueprint = GetWorld()->SpawnActor<AWeaponBase>(WeaponData.Attribute.WeaponClass, ActorSpawnParams);

		m_WeaponBlueprint->AttachToComponent(m_Owner->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
			WeaponData.Attribute.AttachName);

		m_CurrentWeapon = Type;
	}
}

void UWeaponComponent::StartReload()
{
	const FStoredWeapon& storedWeapon = GetWeapon(m_CurrentWeapon);
	if (storedWeapon.CanReload()) {
		//GetWeapon(m_CurrentWeapon).Reload();
		//Play Montage instead


	}
}

void UWeaponComponent::StartShooting()
{
	if (!m_Owner.IsValid())
		return;

	if (!m_WeaponBlueprint.IsValid())
		return;

	const FStoredWeapon& data = GetWeapon(m_CurrentWeapon);

	const FVector TraceStart = m_WeaponBlueprint->GetTraceStart();
	const FVector TraceDirection = m_Owner->GetWeaponTraceDirection() * data.Attribute.TraceDistance;

	for (int i = 0; i < data.Attribute.BulletPerShoot; i++) {

		const float ActualSpreadAngle = data.Attribute.BulletBaseSpreadAngle;
		const float HalfSpreadAngleInRadians = FMath::DegreesToRadians(ActualSpreadAngle * 0.5f);

		const FVector FinalTraceDirection = VRandConeNormalDistribution(TraceDirection, HalfSpreadAngleInRadians, data.Attribute.Exponent);

		TArray<FHitResult> results;
		FHitResult Hit = WeaponTrace(TraceStart, FinalTraceDirection, results, 0.0f);

		/*Apply damage*/
		if (!Hit.GetActor()) {
			PlayShootEffect(FinalTraceDirection);
			continue;
		}
		else {
			PlayShootEffect(Hit.Location);
			PlayHitEffect(Hit.Location);
		}

		if (Hit.BoneName == "head")
			IDamageable::Execute_ApplyDamage(Hit.GetActor(), data.Attribute.Damage * 2);
		else
			IDamageable::Execute_ApplyDamage(Hit.GetActor(), data.Attribute.Damage);
	}
}

FHitResult UWeaponComponent::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, TArray<FHitResult>& OutHitResult, float SweepRadius) const
{
	TArray<FHitResult> HitResults;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/ true, /*IgnoreActor=*/ m_Owner.Get());

	/*ECC_GameTraceChannel2 is weapon trace. Can check DefaultEngine.ini for more detail*/
	const ECollisionChannel TraceChannel = ECC_GameTraceChannel2;

	if (SweepRadius > 0.0f)
	{
		GetWorld()->SweepMultiByChannel(HitResults, StartTrace, EndTrace, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(SweepRadius), TraceParams);
	}
	else
	{
		GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace, TraceChannel, TraceParams);
	}

	if (DebugTime > 0.0f) {

		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, DebugTime, 0, LineThickness);

	}
	FHitResult Hit(ForceInit);
	if (HitResults.Num() > 0)
	{
		// Filter the output list to prevent multiple hits on the same actor;
		// this is to prevent a single bullet dealing damage multiple times to
		// a single actor if using an overlap trace
		for (FHitResult& CurHitResult : HitResults)
		{
			auto Pred = [&CurHitResult](const FHitResult& Other)
				{
					return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
				};

			if (!OutHitResult.ContainsByPredicate(Pred))
			{
				OutHitResult.Add(CurHitResult);
				if (DebugTime > 0.0f)
					DrawDebugPoint(GetWorld(), CurHitResult.ImpactPoint, HitPointThickness, FColor::Green, false, DebugTime);
			}
		}

		Hit = OutHitResult.Last();
	}
	else
	{
		Hit.TraceStart = StartTrace;
		Hit.TraceEnd = EndTrace;
	}

	return Hit;
}

void UWeaponComponent::PlayShootEffect(FVector ShootEndLocation)
{
}

void UWeaponComponent::PlayHitEffect(FVector ShootEndLocation)
{
}


void UWeaponComponent::ReloadCurrentWeapon()
{
	FStoredWeapon& storedWeapon = GetWeapon_Private(m_CurrentWeapon);
	storedWeapon.Reload();
}

void UWeaponComponent::RefillAllAmmunition()
{
	for (FStoredWeapon& weapon : m_WeaponList) {
		weapon.ResetMaxAmmunition();
	}
}

int UWeaponComponent::GetCurrentWeaponAmmunition() const
{
	return GetWeaponAmmunition(m_CurrentWeapon);
}

int UWeaponComponent::GetCurrentMaxWepaonAmmunition() const
{
	return GetMaxWepaonAmmunition(m_CurrentWeapon);
}

EWeaponType UWeaponComponent::GetCurrentWeaponType() const
{
	return m_CurrentWeapon;
}

int UWeaponComponent::GetWeaponAmmunition(const EWeaponType Type) const
{
	const int Out = GetWeapon(Type).CurrentAmmunition;
	return Out;
}

int UWeaponComponent::GetMaxWepaonAmmunition(const EWeaponType Type) const
{
	const int Out = GetWeapon(Type).CurrentMaxAmmunition;
	return Out;
}

const FWeaponAttribute& UWeaponComponent::GetWeaponAttributeRow(const TEnumAsByte<EWeaponType> Type) const
{
	/*Get Weapon Attribute Data*/
	static FWeaponAttribute defaultRow;
	if (!m_DataTableAsset.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponComponent: Invalid Data Asset Reference"));
		return defaultRow;
	}
	FString itemEnumString = UEnum::GetValueAsString(Type);
	/*Convert it to name so we can remove the prefix of enum*/
	FName itemEnumName = UKismetStringLibrary::Conv_StringToName(itemEnumString);

	if (FWeaponAttribute* row = m_DataTableAsset->FindRow<FWeaponAttribute>(itemEnumName, "")) {
		return *row;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("WeaponComponent: Invalid Item Enum %s To Find from DataAsset"), *itemEnumString);
		return defaultRow;
	}

}

const FStoredWeapon& UWeaponComponent::GetWeapon(const EWeaponType Type) const
{
	static const FStoredWeapon DefaultWeapon;

	// Find weapon in the list by type
	const FStoredWeapon* Weapon = m_WeaponList.FindByPredicate([&](const FStoredWeapon& Weapon) {
		return Weapon.Type == Type;
		});

	// Return the found weapon or the default weapon
	return Weapon ? *Weapon : DefaultWeapon;
}

FStoredWeapon& UWeaponComponent::GetWeapon_Private(const EWeaponType Type)
{
	static FStoredWeapon defaultWeapon;
	FStoredWeapon* Weapon = m_WeaponList.FindByPredicate([&](const FStoredWeapon& weapon) {
		return Type == weapon.Type;
		});

	return Weapon ? *Weapon : defaultWeapon;
}



