// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCaster/BlasterTypes/TurningInPlace.h"
#include "BlasterCaster/Interfaces/InteractWithCrosshair.h"
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

	virtual void PostInitializeComponents() override;

	virtual void OnRep_ReplicatedMovement() override;

	

protected:
	virtual void BeginPlay() override;

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
	void PlayHitReactMontage();
	void PlayElimMontage();
	
	void SimProxiesTurn();

	void EquipButtonPressed();
	void CalculateAOPitch();
	void CalculateSpeed(float& Speed) const ;

	void AimOffset(float DeltaTime);

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	virtual void Jump() override;
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

	UPROPERTY(VisibleDefaultsOnly)
	class UCombatComponent* CombatComponent;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	
	UPROPERTY(EditDefaultsOnly, Category="Crouching")
	bool UnCrouchOnReleaseCrouchButton{true};

	float AO_Yaw;
	float AO_Pitch;
	float InterpAOYaw;
	FRotator StartingAimRotation;

	ETurningState TurningState;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditDefaultsOnly,Category=Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly,Category=Combat)
	UAnimMontage* ElimMontage;

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

	UFUNCTION()
	void OnRep_HealthUpdated();

	class ABlasterPlayerController* BlasterPlayerController;

	bool bEliminated = false;

	FTimerHandle EliminatedTimer;

	UPROPERTY(EditDefaultsOnly)
	float EliminatedDelay{3.f};

	void EliminatedTimerFinished();
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
	AWeapon* GetEquippedWeapon();

	void PlayFireMontage(bool bAiming);
	FVector GetHitTarget() const;

	UFUNCTION(NetMulticast,Reliable)
	void MulticastEliminated();

	void Eliminated();
	
};
