// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterAnimInstance.h"
#include "BlasterCharacter.h"
#include "BlasterCaster/Weapons/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!BlasterCharacter)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}

	if(BlasterCharacter == nullptr) return;

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;

	Speed = Velocity.Size();

	bIsRunning = BlasterCharacter->GetIsRunning();

	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelearting = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	WeaponEquipped = BlasterCharacter->GetEquippedWeapon();

	bIsCrouching = BlasterCharacter->bIsCrouched;

	bAiming = BlasterCharacter->IsAiming();

	//Offset Yaw for strafing
	CalculateCharacterRotationAndLean(DeltaSeconds);

	AO_Yaw = BlasterCharacter->GetAOYaw();
	AO_Pitch = BlasterCharacter->GetAOPitch();

	bRotateRootBone = BlasterCharacter->GetShouldRotateRootBone();

	bEliminated = BlasterCharacter->IsEliminated();

	if(bWeaponEquipped && WeaponEquipped && WeaponEquipped->GetWeaponMesh() && BlasterCharacter->GetMesh())
	{
		LeftHandTransform = WeaponEquipped->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"),LeftHandTransform.GetLocation(),FRotator::ZeroRotator,OutPosition,OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if(BlasterCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = WeaponEquipped->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation,LookAtRotation,DeltaSeconds,30.f);
		}
	}

	TurningInPlaceState = BlasterCharacter->GetTurningInPlace();
}

void UBlasterAnimInstance::CalculateCharacterRotationAndLean(float DeltaTime)
{
	//Offset Yaw for strafing
	FRotator AimRotation = BlasterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BlasterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation,AimRotation);

	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRot.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BlasterCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90, 90);
}
