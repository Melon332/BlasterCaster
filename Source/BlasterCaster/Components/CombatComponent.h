// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlasterCaster/Widgets/BlasterHUD.h"
#include "CombatComponent.generated.h"


#define TRACE_LENGTH 80000

class ABlasterCharacter;
class AWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTERCASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	friend class ABlasterCharacter;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	
	void FireButtonPressed(bool bPressed);

	UFUNCTION(Server, Reliable)
	void ServerFireButtonPressed(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireButtonPressed(const FVector_NetQuantize&  TraceHitTarget);

	void TraceUnderCrossHair(FHitResult& HitResult);

	void SetHUDCrosshairs(float DeltaTime);
	
public:
	void EquipWeapon(AWeapon* WeaponToEquip);

private:
	ABlasterCharacter* Character;

	class ABlasterPlayerController* BlasterController;

	class ABlasterHUD* HUD;
	
	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;
	

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditDefaultsOnly)
	float BaseWalkSpeed;
	UPROPERTY(EditDefaultsOnly)
	float AimWalkSpeed;

	bool bFireButtonPressed;

	UPROPERTY(EditAnywhere)
	float OffsetX{15};

	UPROPERTY(EditAnywhere)
	float OffsetY{15};

	//HUD And Crosshair
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	float CrosshairEnemyInSightFactor;

	UPROPERTY(EditDefaultsOnly, Category= "Crosshair Settings")
	float CrosshairAimTightness{0.58f};

	UPROPERTY(EditDefaultsOnly, Category= "Crosshair Settings")
	float CrosshairAimTightnessEnemy{0.9f};

	UPROPERTY(EditDefaultsOnly, Category= "Crosshair Settings")
	float CrosshairShootingSpread{0.2f};

	FVector HitTarget;

	/*
	 * Aiming and FOV
	 */
	//FOV While not aiming set to the cameras base FOV
	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float ZoomedFOV{30};

	float CurrentFOV;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float ZoomInterpSpeed{20};

	void InterpFOV(float DeltaTime);

	UPROPERTY(EditDefaultsOnly)
	FLinearColor EnemyCrosshairColor;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor DefaultCrosshairColor;

	FHUDPackage HUDPackage;

	bool HasEnemyInSight;

	UPROPERTY(EditAnywhere)
	float DistanceToCharacterMax;
};
