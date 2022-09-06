// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "BlasterCaster/Weapons/WeaponTypes.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API AAmmoPickup : public APickup
{
	GENERATED_BODY()
public:
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* PrimitiveComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& HitResult) override;
private:
	UPROPERTY(EditDefaultsOnly)
	int32 AmmoAmount{15};

	UPROPERTY(EditDefaultsOnly)
	EWeaponType WeaponType;
public:
	
	
};
