// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedPickup.h"

#include "BlasterCaster/Character/BlasterCharacter.h"
#include "BlasterCaster/Components/BuffComponent.h"

void ASpeedPickup::OnSphereOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	Super::OnSphereOverlap(PrimitiveComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, HitResult);

	if(ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor))
	{
		
		if(UBuffComponent* BuffComponent = BlasterCharacter->GetBuffComponent())
		{
			BuffComponent->SpeedBuff(BaseSpeedBuff, BaseCrouchSpeedBuff, SpeedBuffTime);
		}
	}
	Destroy();
}