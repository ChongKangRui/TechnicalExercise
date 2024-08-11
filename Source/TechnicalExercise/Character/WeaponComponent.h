#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


UENUM(BlueprintType)
enum EWeaponType : uint8 {
	None,
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute")
	bool HoldToShoot = false;

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

	/*Consider shotgun and some special case*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute")
	int BulletPerShoot = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/Montage")
	TObjectPtr<UAnimMontage> ReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/Montage")
	TObjectPtr<UAnimMontage> SwitchWeaponMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/Montage")
	TObjectPtr<UAnimMontage> ShootMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/VFX")
	TObjectPtr<UNiagaraSystem> Tracer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/VFX")
	TObjectPtr<UNiagaraSystem> Muzzle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/VFX")
	TObjectPtr<UNiagaraSystem> DamageNumber;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Attribute/SFX")
	TObjectPtr<USoundBase> ShootSFX;

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
		return CurrentMaxAmmunition != 0 && CurrentAmmunition < Attribute.MaxAmmunitionPerMagazine;
	}
	void Reload() {
		if (CurrentMaxAmmunition <= Attribute.MaxAmmunitionPerMagazine) {
			CurrentAmmunition = CurrentMaxAmmunition;
			CurrentMaxAmmunition = 0;
		}
		else {
			CurrentMaxAmmunition -= Attribute.MaxAmmunitionPerMagazine - CurrentAmmunition;
			CurrentAmmunition = Attribute.MaxAmmunitionPerMagazine;
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

	/*Weapon Attachment*/
	void SetWeapon(const EWeaponType Type);

	/*Play Reload Montage*/
	void StartReload();

	/*Do actual reload function*/
	void ReloadCurrentWeapon();
	void RefillAllAmmunition();

	/*Start all shooting action, used by player and ai*/
	void StartShooting();
	void StartShooting_Loop();
	void StopShooting_Loop();

	/*Check if continuously shooting*/
	bool IsShooting() const;
	bool CanShoot() const;

	/*Get Specific weapon if needed*/
	const FStoredWeapon& GetWeapon(const EWeaponType Type) const;

	AWeaponBase* GetWeaponBlueprint() const;
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
	bool RandomWeapon = false;

	UPROPERTY(EditDefaultsOnly, Category = DefaultsWeapon, meta = (EditCondition = "RandomWeapon ==  false"))
	TEnumAsByte<EWeaponType> DefaultsWeapon = EWeaponType::Pistol;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBulletHit, AActor*, HitActor);
	FOnBulletHit OnBulletHit;

protected:
	virtual void BeginPlay() override;

	/*VFX and SFX trigger*/
	void PlayShootEffect(const FWeaponAttribute& WeaponAttribute,const FVector& ShootStartLocation, const FVector& ShootEndLocation);
	void PlayHitEffect(const FWeaponAttribute& WeaponAttribute, const FVector& ShootEndLocation, const float Damage);
	
private:
	/*Data Asset getter and initialization*/
	void DataTableAssetInitialization();
	void InitWeaponList();
	const FWeaponAttribute& GetWeaponAttributeRow(const TEnumAsByte<EWeaponType> Type) const;

	/*Core logic of weapon trace*/
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, TArray<FHitResult>& OutHitResult) const;

	/*Get weapon with the availability to change the struct internally*/
	FStoredWeapon& GetWeapon_Private(const EWeaponType Type);
private:
	/*Reference*/
	TArray<FStoredWeapon> m_WeaponList;
	TEnumAsByte<EWeaponType> m_CurrentWeapon = EWeaponType::None;

	TWeakObjectPtr<UDataTable> m_DataTableAsset;
	TWeakObjectPtr<AWeaponBase> m_WeaponBlueprint;
	TWeakObjectPtr<ATechnicalExerciseCharacter> m_Owner;

	FTimerHandle m_ShootTimer;

	bool m_CanShoot;

};
