// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlasterCaster/Widgets/BlasterHUD.h"
#include "BlasterCaster/Weapons/WeaponTypes.h"
#include "BlasterCaster/BlasterTypes/CombatStateTypes.h"
#include "CombatComponent.generated.h"



class ABlasterCharacter;
class AWeapon;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLASTERCASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	friend class ABlasterCharacter;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void FireButtonPressed(bool bPressed);
protected:
	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();
	UFUNCTION()
	void OnRep_SecondaryWeapon();
	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFireButtonPressed(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireButtonPressed(const FVector_NetQuantize&  TraceHitTarget);

	void TraceUnderCrossHair(FHitResult& HitResult);

	void SetHUDCrosshairs(float DeltaTime);
	void DropEquippedWeapon();

	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToLeftHand(AActor* ActorToAttach);
	void AttachActorToBackpack(AActor* ActorToAttach);
	void UpdateCarriedAmmo();
	void PlayEquipWeaponSound(AWeapon* EquippingWeapon);
	void ReloadEmptyWeapon();

	UFUNCTION(Server,Reliable)
	void ServerReload();

	void HandleReload();

	void ThrowGrenade();
	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AProjectile> GrenadeClass;

	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
public:
	void EquipWeapon(AWeapon* WeaponToEquip);
	void SwapWeapons();

	void Reload();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	int32 AmountToReload();
	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();
	void JumpToShotgunEnd();

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();

	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();

	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& Target);

	FORCEINLINE bool IsFiring() const { return bFireButtonPressed; }
	FORCEINLINE int32 GetGrenadesCount() const { return CurrentAmountGrenades; }
	bool CanSwapWeapons();

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);
private:
	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY()
	class ABlasterPlayerController* BlasterController;

	UPROPERTY()
	ABlasterHUD* HUD;
	
	UPROPERTY(ReplicatedUsing=OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(ReplicatedUsing=OnRep_SecondaryWeapon)
	AWeapon* SecondaryWeapon;

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

	FTimerHandle FireHandle;
	bool bCanFire = true;

	void FireTimerFinished();

	void StartFireTimer();

	bool CanFire() const;

	void ToggleGrenadeVisiblity(bool bVisible);

	//Carried ammo for currently equipped weapon
	UPROPERTY(ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditDefaultsOnly)
	int32 StartingARAmmo{30};

	UPROPERTY(EditDefaultsOnly)
	int32 StartingPistolAmmo{60};

	UPROPERTY(EditDefaultsOnly)
	int32 StartingRocketAmmo{0};

	UPROPERTY(EditDefaultsOnly)
	int32 StartingSMGAmmo{0};

	UPROPERTY(EditDefaultsOnly)
	int32 StartingShotgunAmmo{0};

	UPROPERTY(EditDefaultsOnly)
	int32 StartingSniperAmmo{0};

	UPROPERTY(EditDefaultsOnly)
	int32 StartingGrenadeLauncher{0};

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Grenades)
	int32 CurrentAmountGrenades{4};

	UPROPERTY(EditDefaultsOnly)
	int32 MaxCarriedAmmo{500};

	UFUNCTION()
	void OnRep_Grenades();

	UPROPERTY(EditDefaultsOnly)
	int32 MaxGrenades{4};

	void UpdateHUDGrenades();
	
	void InitalizeCarriedAmmo();

	UPROPERTY(ReplicatedUsing=OnRep_CombatState)
	ECombatState CurrentCombatState{ECombatState::ECS_Unoccupied};

	UFUNCTION()
	void OnRep_CombatState();
	
};
