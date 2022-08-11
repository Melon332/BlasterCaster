// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"

#include "NiagaraComponent.h"
#include "BlasterCaster/Character/BlasterCharacter.h"
#include "BlasterCaster/Components/BuffComponent.h"
#include "NiagaraFunctionLibrary.h"

AHealthPickup::AHealthPickup()
{
	bReplicates = true;

	PickupEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Pickup Effect"));
	PickupEffectComponent->SetupAttachment(RootComponent);
}

void AHealthPickup::Destroyed()
{
	if(PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation(), GetActorRotation());
	}

	Super::Destroyed();
}

void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	Super::OnSphereOverlap(PrimitiveComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, HitResult);

	if(ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
	{
		
		if(UBuffComponent* BuffComponent = BlasterCharacter->GetBuffComponent())
		{
			BuffComponent->Heal(HealAmount, HealingTime);
		}
	}
	Destroy();
}
