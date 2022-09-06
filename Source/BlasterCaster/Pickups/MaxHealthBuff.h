// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "MaxHealthBuff.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API AMaxHealthBuff : public APickup
{
	GENERATED_BODY()
public:
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult) override;
private:
	UPROPERTY(EditDefaultsOnly)
	float MaxHealthIncrease{25};

	UPROPERTY(EditDefaultsOnly)
	float BuffDuration{5.f};
};
