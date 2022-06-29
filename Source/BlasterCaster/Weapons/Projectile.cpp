// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BlasterCaster/BlasterCaster.h"
#include "Sound/SoundCue.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisonBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	RootComponent = CollisonBox;

	bReplicates = true;

	CollisonBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisonBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisonBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisonBox->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisonBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisonBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisonBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECR_Block);

	CollisonBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if(Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(Tracer,RootComponent, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}

	if(HasAuthority())
	{
		CollisonBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

void AProjectile::Destroyed()
{
	if(ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}
	
	Super::Destroyed();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

