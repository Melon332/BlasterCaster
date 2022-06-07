// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/BoxComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisonBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	RootComponent = CollisonBox;

	CollisonBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisonBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisonBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisonBox->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisonBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

