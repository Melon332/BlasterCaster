// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"

#include "BlasterCaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"

ARocketProjectile::ARocketProjectile()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARocketProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                              FVector NormalImpulse, const FHitResult& Hit)
{
	APawn* FiringPawn = GetInstigator();
	if(FiringPawn)
	{
		AController* FiringController = FiringPawn->GetController();
		if(FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(this, // World context object
				Damage, //Base Damage
				MinimumDamage, //The minimum damage you will take
				GetActorLocation(), // Origin
				InnerRadius, // The inner radius size
				OuterRadius, // Outer Radius Size
				1.f, //Damage fall off
				UDamageType::StaticClass(),TArray<AActor*>(), //Ignored Actors
				this, // Damage Causer
				FiringController); //InstigatorController
		}
	}
	
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
