// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class BLASTERCASTER_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* PrimitiveComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& HitResult);

	UPROPERTY(EditDefaultsOnly)
	float BaseTurnRate{45.f};
public:	

private:
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* OverlapSphere;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* PickupSound;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* PickupEffectComponent;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* PickupEffect;
};
