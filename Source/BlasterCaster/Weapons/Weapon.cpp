// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "BlasterCaster/Character/BlasterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	SetRootComponent(WeaponMesh);

	bReplicates = true;

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Area Sphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>("PickUpWidget");
	PickUpWidget->SetupAttachment(RootComponent);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if(PickUpWidget)
	{
		PickUpWidget->SetVisibility(false);
	}

	
	if(HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OnComponentEndOverlap,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if(BlasterCharacter)
	{
		BlasterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ShowPickUpWidget(false);
		break;
	}
}

void AWeapon::FireWeapon()
{
	if(FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation,false);
	}
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickUpWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void AWeapon::ShowPickUpWidget(bool bShowWidget)
{
	if(PickUpWidget)
	{
		PickUpWidget->SetVisibility(bShowWidget);
	}
}

