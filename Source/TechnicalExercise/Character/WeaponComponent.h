#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


UENUM(BlueprintType)
enum EWeaponType : uint8 {
	Pistol,
	SMG,
	ShotGun
};

class AWeaponBase;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FWeaponAttribute : public FTableRowBase {
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance", Meta = (ToolTip = "Weapon Blueprint that gonna spawn and attach to character"))
	TSubclassOf<AWeaponBase> WeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance", Meta = (ToolTip = "Weapon Blueprint that gonna spawn and attach to character"))
	FName AttachName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute", Meta = (ToolTip = "Damage Per Bullet/Melee"))
	float Damage = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute")
	int MaxAmmunitionPerMagazine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute")
	int MaxAmmunition;

	/*Maximum shoot trace distance, it can be radius range to grenade*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute")
	float TraceDistance = 10000.0f;

	/*How fast the shooting rate*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute")
	float Rate = 1.0;

	/*I wan to add recoil for gun but unfortunately, I don't have enough time*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute")
	float Recoil = 0.1f;

	/*Consider shotgun and some special case*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute")
	int BulletPerShoot = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/Montage")
	TObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/Montage")
	TObjectPtr<UAnimMontage> SwitchWeaponMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/VFX")
	TObjectPtr<UNiagaraSystem> Tracer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/VFX")
	TObjectPtr<UNiagaraSystem> Muzzle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/SpreadExponent", meta = (EditCondition = "BulletPerShoot > 1"))
	float Exponent = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/SpreadExponent", meta = (EditCondition = "BulletPerShoot > 1"))
	float BulletBaseSpreadAngle = 0;

};
USTRUCT()
struct FStoredWeapon {
	GENERATED_BODY()

public:
	FStoredWeapon() : Type(EWeaponType::Pistol), Attribute(FWeaponAttribute()), CurrentAmmunition(0), CurrentMaxAmmunition(0) {
	}
	FStoredWeapon(EWeaponType type, FWeaponAttribute attribute) : Type(type), Attribute(attribute), CurrentAmmunition(attribute.MaxAmmunitionPerMagazine), CurrentMaxAmmunition(attribute.MaxAmmunition) {
	}

	void ResetAmmunition() {
		CurrentMaxAmmunition = Attribute.MaxAmmunition;
	}
	void ResetMaxAmmunition() {
		CurrentMaxAmmunition = Attribute.MaxAmmunition;
	}
	const bool CanReload() const {
		return CurrentMaxAmmunition != 0;
	}
	void Reload() {
		if (CurrentMaxAmmunition <= Attribute.MaxAmmunitionPerMagazine) {
			CurrentAmmunition = CurrentMaxAmmunition;
			CurrentMaxAmmunition = 0;
		}
		else {
			CurrentAmmunition = Attribute.MaxAmmunitionPerMagazine;
			CurrentMaxAmmunition -= Attribute.MaxAmmunitionPerMagazine;
		}
	}

public:
	EWeaponType Type;
	FWeaponAttribute Attribute;
	int CurrentAmmunition;
	int CurrentMaxAmmunition;
};

class ATechnicalExerciseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TECHNICALEXERCISE_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponComponent();

	//Ammunition Functionality
	UFUNCTION(BlueprintPure)
	int GetCurrentWeaponAmmunition() const;

	UFUNCTION(BlueprintPure)
	int GetCurrentMaxWepaonAmmunition() const;

	UFUNCTION(BlueprintPure)
	EWeaponType GetCurrentWeaponType() const;
	
	int GetWeaponAmmunition(const EWeaponType Type) const;
	int GetMaxWepaonAmmunition(const EWeaponType Type) const;

	/*Weapon functionality*/
	void SetWeapon(const EWeaponType Type);
	/*Play Montage*/
	void StartReload();
	/*Do actual reload function*/
	void ReloadCurrentWeapon();
	void RefillAllAmmunition();
	void StartShooting();

	/*Get Specific weapon if needed*/
	const FStoredWeapon& GetWeapon(const EWeaponType Type) const;

public:
	//Debug Only
	UPROPERTY(EditDefaultsOnly, Category = Debug)
	float DebugTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = Debug)
	float LineThickness = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = Debug)
	float HitPointThickness = 5.0f;

	/*Defaults Weapon List that character can owns*/
	UPROPERTY(EditDefaultsOnly, Category = DefaultsWeapon)
	TArray<TEnumAsByte<EWeaponType>> DefaultsWeaponList = { EWeaponType::Pistol, EWeaponType::SMG, EWeaponType::ShotGun};

	UPROPERTY(EditDefaultsOnly, Category = DefaultsWeapon)
	TEnumAsByte<EWeaponType> DefaultsWeapon;

protected:
	virtual void BeginPlay() override;

	void PlayShootEffect(FVector ShootEndLocation);
	void PlayHitEffect(FVector ShootEndLocation);
	
private:
	/*Data Asset getter and initialization*/
	void DataTableAssetInitialization();
	const FWeaponAttribute& GetWeaponAttributeRow(const TEnumAsByte<EWeaponType> Type) const;
	void InitWeaponList();
	/*Core logic of weapon trace*/
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, TArray<FHitResult>& OutHitResult, float SweepRadius) const;

	/*Get weapon with the availability to change the struct internally*/
	FStoredWeapon& GetWeapon_Private(const EWeaponType Type);
private:
	/*Reference*/
	TArray<FStoredWeapon> m_WeaponList;
	TWeakObjectPtr<UDataTable> m_DataTableAsset;
	TEnumAsByte<EWeaponType> m_CurrentWeapon;
	TWeakObjectPtr<AWeaponBase> m_WeaponBlueprint;

	TWeakObjectPtr<ATechnicalExerciseCharacter> m_Owner;

	bool m_CanShoot = true;

};
