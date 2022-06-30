// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "BlasterCaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AHitScanWeapon::FireWeapon(const FVector& HitTarget)
{
	Super::FireWeapon(HitTarget);


	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(!OwnerPawn) return;
	AController* InstigatorController = OwnerPawn->GetController();
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if(MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget - Start) * 1.25f;
		FHitResult FireHit;
		if(UWorld* World = GetWorld())
		{
			World->LineTraceSingleByChannel(FireHit, Start, End, ECC_Visibility);

			FVector BeamEnd = End;
		
			if(FireHit.bBlockingHit)
			{
				BeamEnd = FireHit.ImpactPoint;
				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
				if(BlasterCharacter && HasAuthority() && InstigatorController)
				{
					UGameplayStatics::ApplyDamage(BlasterCharacter,Damage, InstigatorController, this, UDamageType::StaticClass());
				}
				if(ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
				}
				if(HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(World, HitSound, FireHit.ImpactPoint);
				}
			}
			if(BeamParticles)
			{
				UParticleSystemComponent* BeamParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, SocketTransform);
				if(BeamParticleSystem)
				{
					BeamParticleSystem->SetVectorParameter(FName("Target"), BeamEnd);
				}
			}
			if(MuzzleFlash)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, MuzzleFlash, SocketTransform);
			}
			if(FireSound)
			{
				UGameplayStatics::PlaySoundAtLocation(World, FireSound, GetActorLocation());
			}
		}
	}
}
