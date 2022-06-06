// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterCaster/BlasterTypes/TurningInPlace.h"
#include "BlasterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
protected:
	
private:
	UPROPERTY(BlueprintReadOnly, Category="Character", meta=(AllowPrivateAccess = "true"))
	class ABlasterCharacter* BlasterCharacter;
	
	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	float Speed;
	
	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	bool bIsAccelearting;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	bool bWeaponEquipped;

	class AWeapon* WeaponEquipped;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	bool bIsCrouching;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	bool bAiming;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	float YawOffset;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	float Lean;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	float AO_Yaw;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	float AO_Pitch;

	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	FTransform LeftHandTransform;

	void CalculateCharacterRotationAndLean(float DeltaTime);

	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator DeltaRotation;
	
	UPROPERTY(BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess = "true"))
	ETurningState TurningInPlaceState;
};
