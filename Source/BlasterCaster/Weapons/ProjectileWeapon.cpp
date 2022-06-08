// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::FireWeapon(const FVector& HitTarget)
{
	Super::FireWeapon(HitTarget);

	if(!HasAuthority()) return;
	APawn* PawnInstigator = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(MuzzleSocket && PawnInstigator)
	{
		FTransform MuzzleTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		//From Muzzle flash socket to hit location from trace under crosshair
		FVector ToTarget = HitTarget - MuzzleTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if(ProjectileClass)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetOwner();
			SpawnParameters.Instigator = PawnInstigator;
			GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleTransform.GetLocation(), TargetRotation, SpawnParameters);
		}
	}
}
