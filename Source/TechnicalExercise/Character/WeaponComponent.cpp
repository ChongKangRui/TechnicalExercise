
#include "Character/WeaponComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponBase.h"
#include "DamageSystem/Damageable.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "UI/W_PlayerCrosshair.h"
#include "GameFramework/PlayerState.h"
#include "Character/TechnicalExerciseCharacter.h"
#include "Perception/AISense_Hearing.h"

/*Helper function for bullet spread and exponent*/
FVector VRandConeNormalDistribution(const FVector& Dir, const float ConeHalfAngleRad, const float Exponent)
{
	if (ConeHalfAngleRad > 0.f)
	{
		const float coneHalfAngleDegrees = FMath::RadiansToDegrees(ConeHalfAngleRad);

		// consider the cone a concatenation of two rotations. one "away" from the center line, and another "around" the circle
		// apply the exponent to the away-from-center rotation. a larger exponent will cluster points more tightly around the center
		const float fromCenter = FMath::Pow(FMath::FRand(), Exponent);
		const float angleFromCenter = fromCenter * coneHalfAngleDegrees;
		const float angleAround = FMath::FRand() * 360.0f;

		FRotator Rot = Dir.Rotation();
		FQuat dirQuat(Rot);
		FQuat fromCenterQuat(FRotator(0.0f, angleFromCenter, 0.0f));
		FQuat aroundQuat(FRotator(0.0f, 0.0, angleAround));
		FQuat finalDirectionQuat = dirQuat * aroundQuat * fromCenterQuat;

		finalDirectionQuat.Normalize();

		return finalDirectionQuat.RotateVector(FVector::ForwardVector) * Dir.Size();
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

	/*Initialize reference*/
	if (GetOwner())
		m_Owner = Cast<ATechnicalExerciseCharacter>(GetOwner());

	m_WeaponBlueprint.Reset();

	/*Initialize weapon*/
	InitWeaponList();
	if (!RandomWeapon) {
		SetWeapon(DefaultsWeapon);
	}
	else {
		int weaponToEquip = FMath::RandRange(0, DefaultsWeaponList.Num() - 1);
		SetWeapon(DefaultsWeaponList[weaponToEquip]);
	}
}

void UWeaponComponent::InitWeaponList()
{
	for (EWeaponType type : DefaultsWeaponList) {
		FStoredWeapon weapon(FStoredWeapon(type, GetWeaponAttributeRow(type)));
		m_WeaponList.Add(weapon);
	}
}

void UWeaponComponent::DataTableAssetInitialization()
{
	//Get datatable
	static ConstructorHelpers::FObjectFinder<UDataTable> dataTablePath(TEXT("/Script/Engine.DataTable'/Game/DataAsset/DT_Weapon.DT_Weapon'"));
	m_DataTableAsset = dataTablePath.Object;
}

void UWeaponComponent::SetWeapon(const EWeaponType Type)
{
	if (m_Owner.IsValid()) {

		if (m_CurrentWeapon == Type)
			return;

		const FStoredWeapon& weaponData = GetWeapon(Type);
		if (!weaponData.Attribute.WeaponClass) {
			UE_LOG(LogTemp, Error, TEXT("Invalid Weapon Base Class"));
			return;
		}

		if (m_WeaponBlueprint.IsValid())
			GetWorld()->DestroyActor(m_WeaponBlueprint.Get());

		if (UAnimMontage* m = weaponData.Attribute.SwitchWeaponMontage)
			m_Owner->PlayAnimMontage(m);

		//Spawn weapon and attach it to player hand
		FActorSpawnParameters actorSpawnParams;
		actorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		actorSpawnParams.Owner = m_Owner.Get();
		m_WeaponBlueprint = GetWorld()->SpawnActor<AWeaponBase>(weaponData.Attribute.WeaponClass, actorSpawnParams);

		m_WeaponBlueprint->AttachToComponent(m_Owner->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			weaponData.Attribute.AttachName);

		m_CanShoot = true;

		m_CurrentWeapon = Type;
	}

}

void UWeaponComponent::StartReload()
{
	const FStoredWeapon& storedWeapon = GetWeapon(m_CurrentWeapon);
	if (storedWeapon.CanReload() && !m_Owner->GetMesh()->GetAnimInstance()->Montage_IsPlaying(storedWeapon.Attribute.ReloadMontage)) {

		//Play Montage, the actual value changing will trigger in AnimNotify
		if (UAnimMontage* reloadMontage = GetWeapon(m_CurrentWeapon).Attribute.ReloadMontage)
			m_Owner->PlayAnimMontage(reloadMontage);

		//Disable shooting when weapon ammunition was 0. Otherwise, player can cancel the reload action
		if (GetCurrentWeaponAmmunition() < 0) {
			m_CanShoot = false;
		}

	}
}

void UWeaponComponent::StartShooting()
{
	if (!m_Owner.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("WeaponComponent: Invalid Owner"));
		return;
	}

	if (!m_WeaponBlueprint.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("WeaponComponent: Invalid Weapon Reference"));
		return;
	}

	if (!m_CanShoot) {
		UE_LOG(LogTemp, Error, TEXT("WeaponComponent: Shoot Disable"));
		return;
	}

	FStoredWeapon& data = GetWeapon_Private(m_CurrentWeapon);

	if (data.CurrentAmmunition <= 0) {
		m_CanShoot = false;
		StartReload();
		return;
	}

	const FVector traceStart = m_Owner->GetWeaponTraceStartLocation();
	const FVector traceDirection = m_Owner->GetWeaponTraceEndDirection();

	/*Play Montage*/
	if (UAnimMontage* shootMontage = data.Attribute.ShootMontage)
		m_Owner->PlayAnimMontage(shootMontage);
	/*Play SFX*/
	if (auto shootVFX = data.Attribute.ShootSFX) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), shootVFX, traceStart);
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), m_Owner->GetActorLocation(), 1.0f, m_Owner.Get(), 1000.0f);
	}

	for (int i = 0; i < data.Attribute.BulletPerShoot; i++) {

		const float actualSpreadAngle = data.Attribute.BulletBaseSpreadAngle;
		const float halfSpreadAngleInRadians = FMath::DegreesToRadians(actualSpreadAngle * 0.5f);

		const FVector finalTraceDirection = m_Owner->GetWeaponTraceStartLocation() + VRandConeNormalDistribution(traceDirection, halfSpreadAngleInRadians, data.Attribute.Exponent) * data.Attribute.TraceDistance;

		TArray<FHitResult> results;
		FHitResult hit = WeaponTrace(traceStart, finalTraceDirection, results);

		/*Apply damage and spawn effect*/
		if (!hit.GetActor()) {
			PlayShootEffect(data.Attribute, m_WeaponBlueprint->GetTraceStart(), finalTraceDirection);
			continue;
		}

		PlayShootEffect(data.Attribute, m_WeaponBlueprint->GetTraceStart(), hit.Location);
		if (hit.GetActor()->ActorHasTag("Damageable")) {
			if (!hit.GetActor()->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
				continue;

			if (IDamageable::Execute_GetHealth(hit.GetActor()) <= 0)
				continue;

			//Set final damage to apply
			float finalDamage = 0;
			if (hit.BoneName == "head")
				finalDamage = data.Attribute.Damage * 2;
			else
				finalDamage = data.Attribute.Damage;

			IDamageable::Execute_ApplyDamage(hit.GetActor(), finalDamage, m_Owner.Get());

			PlayHitEffect(data.Attribute, hit.Location, finalDamage);
			OnBulletHit.Broadcast(hit.GetActor());
		}
	}

	data.CurrentAmmunition -= 1;

	/*If not hold to shoot or Is a bot, then reset the m_CanShoot after X Shooting Rate*/
	/*Otherwise, for player, it will using StartShooting_Loop, the functionality below won't trigger*/
	if (!data.Attribute.HoldToShoot || m_Owner->GetPlayerState()->IsABot()) {
		m_CanShoot = false;

		FTimerHandle tempTimeHandle;
		FTimerDelegate tempDelegate;

		tempDelegate.BindWeakLambda(this, [&]()
			{
				m_CanShoot = true;
			});
		GetWorld()->GetTimerManager().SetTimer(tempTimeHandle, tempDelegate, 0.01f, false, data.Attribute.Rate);
	}
}

void UWeaponComponent::StartShooting_Loop()
{
	const FStoredWeapon& data = GetWeapon(m_CurrentWeapon);
	/*For player shoot by hold only*/
	if (data.Attribute.HoldToShoot) {
		FTimerDelegate tempDelegate;
		tempDelegate.BindWeakLambda(this, [&]()
			{
				StartShooting();
				//UE_LOG(LogTemp, Error, TEXT("Shooting"));
			});
		GetWorld()->GetTimerManager().SetTimer(m_ShootTimer, tempDelegate, data.Attribute.Rate, true);
	}
	else {
		StartShooting();
	}
}

void UWeaponComponent::StopShooting_Loop()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(m_ShootTimer)) {
		GetWorld()->GetTimerManager().ClearTimer(m_ShootTimer);
		m_ShootTimer.Invalidate();
	}
}

bool UWeaponComponent::IsShooting() const
{
	return GetWorld()->GetTimerManager().IsTimerActive(m_ShootTimer);
}

bool UWeaponComponent::CanShoot() const
{
	return m_CanShoot;
}

FHitResult UWeaponComponent::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, TArray<FHitResult>& OutHitResult) const
{
	TArray<FHitResult> hitResults;
	FCollisionQueryParams traceParams(SCENE_QUERY_STAT(WeaponTrace), true, m_Owner.Get());

	/*ECC_GameTraceChannel1 is bullet trace. Can check DefaultEngine.ini for more detail*/
	const ECollisionChannel traceChannel = ECC_GameTraceChannel1;

	GetWorld()->LineTraceMultiByChannel(hitResults, StartTrace, EndTrace, traceChannel, traceParams);

	if (DebugTime > 0.0f) {

		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Red, false, DebugTime, 0, LineThickness);

	}
	FHitResult hit(ForceInit);
	if (hitResults.Num() > 0)
	{
		// Filter the output list to prevent multiple hits on the same actor;
		for (FHitResult& curHitResult : hitResults)
		{
			auto pred = [&curHitResult](const FHitResult& Other)
				{
					return Other.HitObjectHandle == curHitResult.HitObjectHandle;
				};

			if (!OutHitResult.ContainsByPredicate(pred))
			{
				OutHitResult.Add(curHitResult);
				if (DebugTime > 0.0f)
					DrawDebugPoint(GetWorld(), curHitResult.ImpactPoint, HitPointThickness, FColor::Green, false, DebugTime);
			}
		}

		hit = OutHitResult.Last();
	}
	else
	{
		hit.TraceStart = StartTrace;
		hit.TraceEnd = EndTrace;
	}

	return hit;
}

void UWeaponComponent::PlayShootEffect(const FWeaponAttribute& WeaponAttribute, const FVector& EffectStartLocation, const FVector& ShootEndLocation)
{
	if (auto tracerSystem = WeaponAttribute.Tracer) {
		UNiagaraComponent* tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, tracerSystem, EffectStartLocation, FRotator::ZeroRotator);

		TArray<FVector> impactLocation = { ShootEndLocation };
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(tracer, "User.ImpactPositions", impactLocation);
	}

	if (auto muzzle = WeaponAttribute.Muzzle) {
		UNiagaraComponent* tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, muzzle, EffectStartLocation, FRotator::ZeroRotator);
	}

}

void UWeaponComponent::PlayHitEffect(const FWeaponAttribute& WeaponAttribute, const FVector& ShootEndLocation, const float Damage)
{
	if (auto damageNumber = WeaponAttribute.DamageNumber) {
		UNiagaraComponent* tracer = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, damageNumber, ShootEndLocation, FRotator::ZeroRotator);

		TArray<FVector4> damageInfo = { FVector4(ShootEndLocation, Damage) };
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(tracer, "User.DamageInfo", damageInfo);
	}
}

void UWeaponComponent::ReloadCurrentWeapon()
{
	FStoredWeapon& storedWeapon = GetWeapon_Private(m_CurrentWeapon);
	storedWeapon.Reload();
	m_CanShoot = true;
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
	const int out = GetWeapon(Type).CurrentAmmunition;
	return out;
}

int UWeaponComponent::GetMaxWepaonAmmunition(const EWeaponType Type) const
{
	const int out = GetWeapon(Type).CurrentMaxAmmunition;
	return out;
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
	static const FStoredWeapon defaultWeapon;

	const FStoredWeapon* weapon = m_WeaponList.FindByPredicate([&](const FStoredWeapon& WeaponAttribute) {
		return Type == WeaponAttribute.Type;
		});

	return weapon ? *weapon : defaultWeapon;
}

AWeaponBase* UWeaponComponent::GetWeaponBlueprint() const
{
	return m_WeaponBlueprint.Get();
}

FStoredWeapon& UWeaponComponent::GetWeapon_Private(const EWeaponType Type)
{
	static FStoredWeapon defaultWeapon;

	FStoredWeapon* weapon = m_WeaponList.FindByPredicate([&](const FStoredWeapon& WeaponAttribute) {
		return Type == WeaponAttribute.Type;
		});

	return weapon ? *weapon : defaultWeapon;;
}



