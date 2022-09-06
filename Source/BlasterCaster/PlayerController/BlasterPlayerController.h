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
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeat(int32 Deaths);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDGrenades(int32 Grenades);
	void SetHUDWeaponName(FString WeaponName, FString WeaponType);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDWarmupCountdown(float WarmupCountdown);
	void ActivateEliminatedText();
	void SetLastDefeatName(FString PlayerName);
	void DeactivateEliminatedText();
	void ChangeColorOfText(float Minutes, float Seconds, class UTextBlock* TextBlock);
	void OnMatchStateSet(FName State);
	virtual void BeginPlayingState() override;
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

	void HandleMatchHasStarted();
	void HandleCooldownHasStarted();

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

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaSeconds);
private:
	UPROPERTY()
	class ABlasterHUD* BlasterHUD;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime{0};
	float WarmupTime{0};
	float CooldownTime{0};
	float LevelStartingTime{0};
	uint32 CountdownInt{0};

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;

	UPROPERTY(EditDefaultsOnly)
	float TimerBlinking{30};

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	

	float HUDHealth;
	float HUDMaxHealth;
	bool bInitHealth{false};
	
	float HUDShield;
	float HUDMaxShield;
	bool bInitShield{false};

	float HUDCarriedAmmo;
	bool bInitCarriedAmmo{false};

	float HUDWeaponAmmo;
	bool bInitWeaponAmmo{false};

	float HUDScore;
	bool bInitScore{false};
	int32 HUDDefeats;
	bool bInitDefeats{false};
	int32 HUDGrenades;
	bool bInitGrenades{false};
	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode;

	UPROPERTY(EditDefaultsOnly)
	FLinearColor ColorWhenBlinking;

	FTimerHandle TimerHandle;

	UPROPERTY(Replicated)
	FString LastLostToPlayerName;

	float HighPingRunningTime{0.f};
	
	UPROPERTY(EditDefaultsOnly)
	float HighPingDuration{5.f};

	UPROPERTY(EditDefaultsOnly)
	float CheckPingFrequency{20.f};

	UPROPERTY(EditDefaultsOnly)
	float HighPingThreshold{50.f};

	float PingAnimationRunningTime{0.f};
};
