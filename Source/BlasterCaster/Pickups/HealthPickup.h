// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API AHealthPickup : public APickup
{
	GENERATED_BODY()
public:
	AHealthPickup();
	virtual void Destroyed() override;
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult) override;
private:
	UPROPERTY(EditDefaultsOnly)
	float HealAmount{25};

	UPROPERTY(EditDefaultsOnly)
	float HealingTime{5.f};

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* PickupEffectComponent;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* PickupEffect;
};
