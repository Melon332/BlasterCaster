// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()
public:
	virtual void FireWeapon(const FVector& HitTarget) override;
protected:
	
private:
	UPROPERTY(EditDefaultsOnly, Category=WeaponScatter)
	uint32 NumberOfPellets{10};
	
};
