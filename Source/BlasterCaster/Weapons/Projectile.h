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

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* TrailSystem;

	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ProjectileMesh;

	void SpawnTrailSystem();
	void DestroyTimerFinished();
	FTimerHandle DestroyTimer;
	void StartDestroyTimer();

	void ExplosionDamage();

	UPROPERTY(EditDefaultsOnly)
	float DestroyTime{3};
	
	UPROPERTY(EditDefaultsOnly)
	float InnerRadius = 200.f;

	UPROPERTY(EditDefaultsOnly)
	float OuterRadius{500.f};

	UPROPERTY(EditDefaultsOnly)
	float MinimumDamage{10.f};
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:


	
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* Tracer;
	
	
	class UParticleSystemComponent* TracerComponent;
	
};
