// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

/**
 * 
 */
class ABlasterPlayerState;
UCLASS()
class BLASTERCASTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()
public:
	void virtual GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(ABlasterPlayerState* ScoringPlayer);
	UPROPERTY(Replicated)
	TArray<ABlasterPlayerState*> TopBlasterPlayers;
protected:
private:
	float TopScore{0.f};
};
