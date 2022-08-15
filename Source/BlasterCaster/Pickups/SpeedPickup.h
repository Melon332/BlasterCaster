// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "SpeedPickup.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ASpeedPickup : public APickup
{
	GENERATED_BODY()
public:
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult) override;
private:
	UPROPERTY(EditDefaultsOnly)
	float BaseSpeedBuff{1200.f};

	UPROPERTY(EditDefaultsOnly)
	float BaseCrouchSpeedBuff{700.f};

	UPROPERTY(EditDefaultsOnly)
	float SpeedBuffTime{30.f};
};
