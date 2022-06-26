// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeat(int32 Deaths);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDWeaponName(FString WeaponName, FString WeaponType);
	void SetHUDMatchCountdown(float CountdownTime);
	void ActivateEliminatedText();
	void DeactivateEliminatedText();
	void OnMatchStateSet(FName State);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetServerTime(); //Synced with server world clock
	virtual void ReceivedPlayer() override;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;
	void SetHUDTime();
	void CheckTimeSync(float DeltaSeconds);
	
	void PollInit();

	/*
	 *Sync time between client and server
	 */

	//Requests the current server time passing in the clients time when the request was sent.
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float ClientTime);

	//Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client,Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta{0.f}; //Difference between client and server time

	UPROPERTY(EditDefaultsOnly, Category=Time)
	float TimeSyncFrequency{5.f}; //Sync up with server time

	float TimeSyncRunningTime{0.f}; //Current sync time
private:
	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime{120.f};
	uint32 CountdownInt{0};

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	bool bInitializeCharacterOverlay{false};

	float HUDHealth;
	float HUDMaxHealth;

	float HUDScore;
	int32 HUDDefeats;
};
