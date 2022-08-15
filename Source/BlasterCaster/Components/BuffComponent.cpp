// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"

#include "BlasterCaster/Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HealRampUp(DeltaTime);
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;

	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;
}

void UBuffComponent::HealRampUp(float DeltaTime)
{
	if(!bHealing || Character == nullptr || Character->IsEliminated()) return;

	const float HealThisFrame = HealingRate * DeltaTime;

	Character->SetCurrentHealth(FMath::Clamp(Character->GetCurrentHealth() + HealThisFrame, 0, Character->GetMaxHealth()));
	AmountToHeal -= HealThisFrame;
	Character->UpdateHUDHealth();

	
	if(AmountToHeal <= 0 || Character->GetCurrentHealth() >= Character->GetMaxHealth())
	{
		bHealing = false;
		AmountToHeal = 0;
	}
}

void UBuffComponent::SpeedBuff(float SpeedBuff, float CrouchSpeedBuff, float SpeedBuffTime)
{
	if(!Character) return;
	Character->GetWorldTimerManager().SetTimer(SpeedBuffHandle, this, &ThisClass::ResetSpeed, SpeedBuffTime);

	if(UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement())
	{
		CharacterMovementComponent->MaxWalkSpeed = SpeedBuff;
		CharacterMovementComponent->MaxWalkSpeedCrouched = CrouchSpeedBuff;
	}
	MulticastSpeedBuff(SpeedBuff, CrouchSpeedBuff);
}

void UBuffComponent::ResetSpeed()
{
	if(!Character) return;
	if(UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement())
	{
		CharacterMovementComponent->MaxWalkSpeed = InitialBaseSpeed;
		CharacterMovementComponent->MaxWalkSpeedCrouched = InitialCrouchSpeed;
	}
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed);
}

void UBuffComponent::MulticastSpeedBuff_Implementation(float SpeedBuff, float CrouchSpeedBuff)
{
	if(!Character) return;
	if(UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement())
	{
		CharacterMovementComponent->MaxWalkSpeed = SpeedBuff;
		CharacterMovementComponent->MaxWalkSpeedCrouched = CrouchSpeedBuff;
	}
}

