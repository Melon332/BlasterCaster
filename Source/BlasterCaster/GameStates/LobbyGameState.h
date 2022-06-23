// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()
public:
	UPROPERTY(ReplicatedUsing=OnRep_PlayersAdded)
	int32 Players{0};
	
	UPROPERTY(Replicated)
	int32 MaxPlayers{};

	void IncreasePlayers(int32 MaximumPlayers);
	void DecreasePlayers();
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
private:
	
	UFUNCTION()
	void OnRep_PlayersAdded();
};
