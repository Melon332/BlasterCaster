// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "Math/UnrealMathUtility.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	BulletCase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Casing"));
	SetRootComponent(BulletCase);
	BulletCase->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	BulletCase->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);
	BulletCase->SetSimulatePhysics(true);
	BulletCase->SetEnableGravity(true);
	BulletCase->SetNotifyRigidBodyCollision(true);

	ShellEjectionImpulse = 5;
	LifeTime = 5;
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();

	BulletCase->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
	BulletCase->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);

	SetLifeSpan(LifeTime);

	SetActorRotation(UKismetMathLibrary::RandomRotator(true));
}

void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if(ShellSound && !HasHitFloor)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
		HasHitFloor = true;
	}
}
