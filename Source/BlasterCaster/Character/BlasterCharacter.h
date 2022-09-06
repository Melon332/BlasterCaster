// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterCaster/BlasterTypes/CombatStateTypes.h"
#include "GameFramework/Character.h"
#include "BlasterCaster/BlasterTypes/TurningInPlace.h"
#include "BlasterCaster/Interfaces/InteractWithCrosshair.h"
#include "Components/TimelineComponent.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTERCASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshair
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateHUDHealth();
	void UpdateHUDShield();

	virtual void PostInitializeComponents() override;

	virtual void OnRep_ReplicatedMovement() override;

	virtual void Destroyed() override;
	
	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	void SpawnDefaultWeapon();

	UPROPERTY(Replicated)
	bool bDisableGameplay{false};
protected:
	virtual void BeginPlay() override;
	void RotateInPlace(float DeltaTime);

	void MoveForward(float value);
	void MoveRight(float value);
	void Turn(float value);
	void LookUp(float value);
	void CrouchButtonPressed();
	void CrouchButtonReleased();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void ReloadButtonPressed();
	void PlayHitReactMontage();
	void PlayElimMontage();
	void UpdateHUDAmmo();
	void DropOrDestroyWeapon(class AWeapon* Weapon);
	void DropOrDestroyWeapons();

	void GrenadeButtonPressed();
	
	void SimProxiesTurn();

	void EquipButtonPressed();
	void CalculateAOPitch();
	void CalculateSpeed(float& Speed) const ;

	void AimOffset(float DeltaTime);

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	virtual void Jump() override;

	virtual void Landed(const FHitResult& Hit) override;

	//Initalize any data that doesn't get initalized in begin play
	void PollInit();
private:
	UPROPERTY(EditDefaultsOnly,Category="Camera")
	class USpringArmComponent* CameraBoom;
	UPROPERTY(EditDefaultsOnly,Category="Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly)
	float Sensitivty{2};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UWidgetComponent* OverHeadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UCombatComponent* CombatComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	class UBuffComponent* BuffComponent;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	
	UPROPERTY(EditDefaultsOnly, Category="Crouching")
	bool UnCrouchOnReleaseCrouchButton{true};

	float AO_Yaw;
	float AO_Pitch;
	float InterpAOYaw;
	FRotator StartingAimRotation;

	UPROPERTY(Replicated)
	bool bIsRunning;

	ETurningState TurningState;
	void TurnInPlace(float DeltaTime);

	/*
	 *Animation Montages
	 */
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditDefaultsOnly,Category=Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly,Category=Combat)
	UAnimMontage* ElimMontage;

	UPROPERTY(EditDefaultsOnly,Category=Combat)
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly,Category=Combat)
	UAnimMontage* GrenadeMontage;

	void HideCharacterIfCharacterClose();

	UPROPERTY(EditDefaultsOnly)
	float CameraThreshold{200};

	bool bRotateRootBone;

	UPROPERTY(EditDefaultsOnly)
	float TurnThreshold{0.5f};

	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;

	float ProxyYaw;

	float TimeSinceLastMovementRep;
	/**
	* Player Health
	*/
	UPROPERTY(EditDefaultsOnly, Category="Player Stats")
	float MaxHealth{100.f};
	UPROPERTY(ReplicatedUsing=OnRep_HealthUpdated, VisibleAnywhere)
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, Category="Player Stats")
	float MaxShield{100.f};
	UPROPERTY(ReplicatedUsing=OnRep_ShieldUpdated, EditDefaultsOnly)
	float CurrentShield{100.f};

	/*
	 *Dissolve Effect
	 */
	FOnTimelineFloat DissolveTrack;
	
	UPROPERTY(VisibleDefaultsOnly)
	UTimelineComponent* DissolveTimeLine;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* DissolveCurve;
	
	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	//Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category="Eliminated")
	UMaterialInstanceDynamic* EliminatedDynamicMaterialInstance;
	
	//Material instance set on blueprint, used with dynamic instance material
	UPROPERTY(EditDefaultsOnly, Category= "Eliminated")
	UMaterialInstance* EliminatedMaterialInstance;

	
	UFUNCTION()
	void OnRep_HealthUpdated(float LastHealth);

	UFUNCTION()
	void OnRep_ShieldUpdated(float LastShield);

	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController;

	bool bEliminated = false;

	FTimerHandle EliminatedTimer;

	UPROPERTY(EditDefaultsOnly)
	float EliminatedDelay{3.f};

	void EliminatedTimerFinished();

	void StartSprinting();
	void StopSprinting();

	UPROPERTY(EditDefaultsOnly)
	float SprintSpeed;

	UPROPERTY(EditDefaultsOnly)
	float WalkSpeed;

	/*
	 * Elimination Sound and Bot
	 */
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* EliminationBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ParticleSystemComponentBot;

	UPROPERTY(EditDefaultsOnly)
	class USoundCue* ElimBotSound;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;
	/*
	 * Grenade
	 */
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* GrenadeMesh;

	//Default Weapon
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> DefaultWeaponClass;
public:	
	void SetOverlappingWeapon(AWeapon* OverlappedWeapon);
	bool IsWeaponEquipped();
	bool IsAiming();

	FORCEINLINE float GetAOYaw() const { return AO_Yaw; }
	FORCEINLINE float GetAOPitch() const { return AO_Pitch; }
	FORCEINLINE ETurningState GetTurningInPlace() const { return TurningState; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool GetShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsEliminated() const { return bEliminated; }
	FORCEINLINE bool GetIsRunning() const { return bIsRunning; }
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetCurrentShield() const { return CurrentShield; }
	FORCEINLINE void SetCurrentHealth(float Amount) { CurrentHealth = Amount; }
	FORCEINLINE void SetCurrentShield(float Amount) { CurrentShield = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE UBuffComponent* GetBuffComponent() const { return BuffComponent; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetGrenadeMesh() const { return GrenadeMesh; }
	ECombatState GetCurrentCombatState() const;
	AWeapon* GetEquippedWeapon();

	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayThrowGrenadeMontage();
	FVector GetHitTarget() const;

	UFUNCTION(NetMulticast,Reliable)
	void MulticastEliminated();

	UFUNCTION(Server,Reliable)
	void ServerSprinting(bool bRunning);

	void Eliminated();

	bool DiedFromFalling{false};

	void SetDiedFromFalling(bool bDiedFromFalling) { DiedFromFalling = bDiedFromFalling; }

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);
};

