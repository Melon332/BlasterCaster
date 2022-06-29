// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTERCASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void Destroyed() override;

	UPROPERTY(EditDefaultsOnly)
	float Damage{20.f};
	
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* ImpactSound;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* CollisonBox;
	
	UPROPERTY(VisibleDefaultsOnly)
	class UProjectileMovementComponent* ProjectileMovementComponent;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:


	
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* Tracer;
	
	
	class UParticleSystemComponent* TracerComponent;
	
};
