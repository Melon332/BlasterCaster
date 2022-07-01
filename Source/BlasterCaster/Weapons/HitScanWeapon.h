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
	virtual FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	void WeaponTraceHit(const FVector& Start, const FVector& End, FHitResult& OutHit);
	
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* BeamParticles;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* HitSound;

	UPROPERTY(EditDefaultsOnly)
	float Damage{8};

private:
	
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* MuzzleFlash;

	
	UPROPERTY(EditDefaultsOnly)
	class USoundCue* FireSound;
	
	/*
	 * Trace end with scatter
	 */
	UPROPERTY(EditDefaultsOnly,Category=WeaponScatter)
	float DistanceToSphere{800.f};

	UPROPERTY(EditDefaultsOnly,Category=WeaponScatter)
	float SphereRadius{75.f};

	UPROPERTY(EditDefaultsOnly, Category=WeaponScatter)
	bool bUseScatter{false};
};
