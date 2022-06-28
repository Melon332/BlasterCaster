// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

namespace MatchState
{
	extern BLASTERCASTER_API const FName AllPlayersReady;
}
/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	virtual void OnMatchStateSet() override;

	void ForceStart();
	virtual void Tick(float DeltaSeconds) override;

	void LoadLevel(FString LevelAddress);

	float CurrentCountdownTime{0.f};

	UPROPERTY(EditDefaultsOnly)
	float LevelStartingTimer{50.f};
protected:
private:
	UPROPERTY(EditDefaultsOnly)
	int32 MaxPlayers{4};

	float LevelLoadedStartingTime{0.f};
};
