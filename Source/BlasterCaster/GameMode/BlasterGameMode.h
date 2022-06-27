// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define DISTANCE_BETWEEN_PLAYER 500.f;
#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

namespace MatchState
{
	extern BLASTERCASTER_API const FName Cooldown; //Match duration has been reached, display winner and begin cooldown timer.
}

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ABlasterGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerEliminated(class ABlasterCharacter* EliminatedPlayer, class ABlasterPlayerController* EliminatedPlayerController, ABlasterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* EliminatedPlayer, AController* EliminatedController);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime {5.f};

	UPROPERTY(EditDefaultsOnly)
	float MatchTime{120.f};
	
	UPROPERTY(EditDefaultsOnly)
	float CooldownDuration{10.f};

	float LevelStartingTime{0.f};

	FORCEINLINE float GetCountDownTime() const { return CountdownTime; }
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
private:
	AActor* FindRandomSpawnPosition(AActor* EliminatedPlayer);

	UPROPERTY(EditDefaultsOnly)
	float DistanceBetweenPlayers{200.f};

	float CountdownTime{0.f};
};
