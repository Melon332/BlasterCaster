// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "RocketProjectile.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ARocketProjectile : public AProjectile
{
	GENERATED_BODY()
public:
	ARocketProjectile();
	virtual void Destroyed() override;
protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;

	void DestroyTimerFinished();

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* TrailSystem;

	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* ProjectileLoop;

	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent;

	UPROPERTY(EditDefaultsOnly)
	USoundAttenuation* LoopingSoundAttenuation;

	UPROPERTY(VisibleDefaultsOnly)
	class URocketMovementComponent* RocketMovementComponent;
private:
	UPROPERTY(EditDefaultsOnly)
	float InnerRadius = 200.f;

	UPROPERTY(EditDefaultsOnly)
	float OuterRadius{500.f};

	UPROPERTY(EditDefaultsOnly)
	float MinimumDamage{10.f};

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* RocketMesh;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditDefaultsOnly)
	float DestroyTime{3};
};

