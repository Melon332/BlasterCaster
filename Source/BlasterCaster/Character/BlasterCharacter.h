// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTERCASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABlasterCharacter();

	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);
	void Turn(float value);
	void LookUp(float value);

	void EquipButtonPressed();
private:
	UPROPERTY(EditDefaultsOnly,Category="Camera")
	class USpringArmComponent* CameraBoom;
	UPROPERTY(EditDefaultsOnly,Category="Camera")
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditDefaultsOnly)
	float Sensitivty{25};

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
	
public:	
	void SetOverlappingWeapon(AWeapon* OverlappedWeapon);
	bool IsWeaponEquipped();
};
