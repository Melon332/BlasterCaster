// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY()
	class ALobbyHUD* LobbyHUD;
	
	void UpdatePlayerAmount();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnMatchStateSet(FName MatchState);
protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
private:
	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName CurrentMatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UFUNCTION(Server,Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientGetMatchStateInformation(FName MatchState, float Timer, float StartingTime);

	void HandleAllPlayersJoined();

	void UpdateAnnouncementTimer(float Timer);

	float LevelStartingTimer{0.f};

	float CurrentTimer{0.f};

	UPROPERTY(EditDefaultsOnly)
	float TimerBlinking{5.f};

	UPROPERTY(EditDefaultsOnly)
	FLinearColor ColorWhenBlinking;

	void ChangeColorOfText(float Minutes, float Seconds, class UTextBlock* TextBlock);
};
