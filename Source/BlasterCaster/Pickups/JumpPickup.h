// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "JumpPickup.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API AJumpPickup : public APickup
{
	GENERATED_BODY()
public:
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult) override;
private:
	UPROPERTY(EditDefaultsOnly)
	float JumpZVelocityBuff{4000.f};

	UPROPERTY(EditDefaultsOnly)
	float JumpBuffTime{30.f};
};
