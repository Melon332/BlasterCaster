// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "ShieldPickup.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API AShieldPickup : public APickup
{
	GENERATED_BODY()
public:
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult) override;
private:
	UPROPERTY(EditDefaultsOnly)
	float ShieldReplenishAmount{25};

	UPROPERTY(EditDefaultsOnly)
	float ShieldReplenishTime{5.f};
};
