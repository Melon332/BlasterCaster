// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTERCASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffComponent();
	friend class ABlasterCharacter;

protected:
	virtual void BeginPlay() override;

	void HealRampUp(float DeltaTime);
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Heal(float HealAmount, float HealingTime);
	void SpeedBuff(float SpeedBuff, float CrouchSpeedBuff, float SpeedBuffTime);
private:
	UPROPERTY()
	ABlasterCharacter* Character;

	//HEALING VARIABLES
	bool bHealing{false};
	float HealingRate{0.f};
	float AmountToHeal{0.f};

	//SPEED VARIABLES
	FTimerHandle SpeedBuffHandle;
	void ResetSpeed();

	float InitialBaseSpeed;
	float InitialCrouchSpeed;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float SpeedBuff, float CrouchSpeedBuff);
public:
	FORCEINLINE void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed) { InitialBaseSpeed = BaseSpeed; InitialCrouchSpeed = CrouchSpeed; }
};
