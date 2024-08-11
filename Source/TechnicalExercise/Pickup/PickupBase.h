// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class USphereComponent;
class UWidgetComponent;
class UW_StartTimer;

UCLASS()
class TECHNICALEXERCISE_API APickupBase : public AActor
{
	GENERATED_BODY()

protected:
	APickupBase();

	void BeginPlay() override;

	// Function to handle the overlap event
	// Bind it in base class but the functionality will be override in derive class
	UFUNCTION()
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void StartPickupCoolDown();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USphereComponent> SphereCollisionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> WidgetComponent;

	UPROPERTY(EditDefaultsOnly)
	float WidgetHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly)
	float PickupCD = 30.0f;

	bool CanPickup;
private:
	TObjectPtr<UW_StartTimer> m_CoolDownTimer;
};
