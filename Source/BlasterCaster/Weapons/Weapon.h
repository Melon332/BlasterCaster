// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlasterCaster/Weapons/WeaponTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	
	EWS_MAX UMETA(DisplayName = "Default Max")
};

class USphereComponent;
UCLASS()
class BLASTERCASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickUpWidget(bool bShowWidget);
	void Dropped();
	void AddAmmo(int32 AmmoToAdd);
	void PlayReloadWeaponAnimation();

	/*
	 * Enable Or Disable Custom Depth
	 */
	void ToggleCustomDepth(bool bEnable);
protected:
	virtual void BeginPlay() override;

	virtual void OnRep_Owner() override;
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* PrimitiveComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& HitResult);
	
	UFUNCTION()
	virtual void OnSphereEndOverlap(
		UPrimitiveComponent* OnComponentEndOverlap,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	void SpendRound();

public:
	
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV;}
	FORCEINLINE float GetZoomedInterpSpeed() const { return ZoomInterpTime; }
	FORCEINLINE float GetFireRate() const { return FireDelay; }
	
	FORCEINLINE bool GetIsAutomatic() const { return bAutomatic; }
	FORCEINLINE bool IsEmpty() const { return CurrentAmmo <= 0; }
	FORCEINLINE bool IsFull() const { return CurrentAmmo >= MaxMagCapacity; }
	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmo; }
	FORCEINLINE int32 GetMaxMagCapacity() const { return MaxMagCapacity; }
	FORCEINLINE EWeaponType GetWeaponType() const { return CurrentWeaponType; }
	FORCEINLINE FString GetWeaponName() const { return WeaponName; }
	
	virtual void FireWeapon(const FVector& HitTarget);

	void UpdateAmmoHUD();
	
	//Textures for the weapon crosshairs
	UPROPERTY(EditDefaultsOnly, Category="Crosshairs")
	class UTexture2D* CrosshairCenter;

	UPROPERTY(EditDefaultsOnly, Category="Crosshairs")
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditDefaultsOnly, Category="Crosshairs")
	UTexture2D* CrosshairRight;

	UPROPERTY(EditDefaultsOnly, Category="Crosshairs")
	UTexture2D* CrosshairTop;

	UPROPERTY(EditDefaultsOnly, Category="Crosshairs")
	UTexture2D* CrosshairBottom;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* EquipSound;
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category="Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleDefaultsOnly, Category="Weapon Properties")
	USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties", ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState;

	UPROPERTY(VisibleDefaultsOnly, Category="Weapon Properties")
	class UWidgetComponent* PickUpWidget;
	
	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditDefaultsOnly, Category="Weapon Properties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditDefaultsOnly, Category="Weapon Properties")
	UAnimationAsset* ReloadAnimation;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACasing> CasingClass;

	//Zoomed FOV while aiming
	UPROPERTY(EditDefaultsOnly)
	float ZoomedFOV{30};

	UPROPERTY(EditDefaultsOnly)
	float ZoomInterpTime{10};

	UPROPERTY(EditAnywhere, Category="Combat")
	float FireDelay{0.2f};

	UPROPERTY(EditAnywhere, Category="Combat")
	bool bAutomatic;

	UPROPERTY(VisibleDefaultsOnly, ReplicatedUsing=OnRep_Ammo)
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxMagCapacity;

	UFUNCTION()
	void OnRep_Ammo();

	UPROPERTY()
	class ABlasterCharacter* BlasterOwnerCharacter;
	
	UPROPERTY()
	class ABlasterPlayerController* BlasterOwnerController;

	UPROPERTY(EditDefaultsOnly)
	EWeaponType CurrentWeaponType;

	UPROPERTY(EditDefaultsOnly)
	FString WeaponName;
};
