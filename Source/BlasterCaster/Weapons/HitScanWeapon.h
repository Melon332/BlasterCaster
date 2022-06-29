// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void FireWeapon(const FVector& HitTarget) override;
protected:
private:

	UPROPERTY(EditDefaultsOnly)
	float Damage{8};

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* ImpactParticles;
};
