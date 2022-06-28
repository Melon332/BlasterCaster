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
protected:
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
private:
	UPROPERTY(EditDefaultsOnly)
	float InnerRadius = 200.f;

	UPROPERTY(EditDefaultsOnly)
	float OuterRadius{500.f};

	UPROPERTY(EditDefaultsOnly)
	float MinimumDamage{10.f};

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* RocketMesh;
};

