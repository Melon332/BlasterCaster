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
	ShieldReplenishRampUp(DeltaTime);
}

void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true;

	HealingRate = HealAmount / HealingTime;
	AmountToHeal += HealAmount;
}

void UBuffComponent::ReplenishShield(float ShieldAmount, float ShieldTime)
{
	bReplenishShield = true;

	ReplenishShieldRate = ShieldAmount / ShieldTime;
	ShieldReplenishAmount += ShieldAmount;
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

void UBuffComponent::ShieldReplenishRampUp(float DeltaTime)
{
	if(!bReplenishShield || Character == nullptr || Character->IsEliminated()) return;

	const float ReplenishShieldThisFrame = ReplenishShieldRate * DeltaTime;

	Character->SetCurrentShield(FMath::Clamp(Character->GetCurrentShield() + ReplenishShieldThisFrame, 0, Character->GetMaxShield()));
	Character->UpdateHUDShield();
	ShieldReplenishAmount -= ReplenishShieldThisFrame;
	
	if(ShieldReplenishAmount <= 0 || Character->GetCurrentShield() >= Character->GetMaxShield())
	{
		bReplenishShield = false;
		ShieldReplenishAmount = 0;
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

void UBuffComponent::ResetJump()
{
	if(!Character) return;
	if(UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement())
	{
		CharacterMovementComponent->JumpZVelocity = InitialBaseJump;
	}
	MulticastJumpBuff(InitialBaseJump);
}

void UBuffComponent::BuffJump(float BuffJumpVelocity, float BuffTime)
{
	if(!Character) return;

	if(UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement())
	{
		CharacterMovementComponent->JumpZVelocity = BuffJumpVelocity;
	}
	Character->GetWorldTimerManager().SetTimer(SpeedBuffHandle, this, &ThisClass::ResetJump, BuffTime);
	MulticastJumpBuff(BuffJumpVelocity);
}

void UBuffComponent::MaxHealthIncrease(float NewMaxHealth, float BuffTime)
{
	if(!Character) return;

	Character->SetMaxHealth(Character->GetMaxHealth() + NewMaxHealth);

	float CurrentHealthHeal = Character->GetMaxHealth() - Character->GetCurrentHealth();
	Heal(CurrentHealthHeal, 1);
	Character->GetWorldTimerManager().SetTimer(MaxHealthBuffHandle, this , &ThisClass::MaxHealthBuffTimerFinished, BuffTime);
}

void UBuffComponent::MaxHealthBuffTimerFinished()
{
	if(!Character) return;
	Character->SetMaxHealth(Character->GetDefaultMaxHealth());

	if(Character->GetCurrentHealth() >= Character->GetMaxHealth())
	{
		Character->SetCurrentHealth(Character->GetMaxHealth());
	}
	Character->UpdateHUDHealth();

	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Orange, TEXT("I am de-buffed now"));
	}
}

void UBuffComponent::MulticastJumpBuff_Implementation(float JumpBuff)
{
	if(!Character) return;
	if(UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement())
	{
		CharacterMovementComponent->JumpZVelocity = JumpBuff;
	}
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


