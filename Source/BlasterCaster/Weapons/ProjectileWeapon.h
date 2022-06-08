// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	virtual void FireWeapon(const FVector& HitTarget) override;
protected:
	
private:
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon Properties")
	TSubclassOf<class AProjectile> ProjectileClass;
};
