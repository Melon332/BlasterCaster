// Copyright Epic Games, Inc. All Rights Reserved.


#include "BlasterCasterGameModeBase.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ABlasterCasterGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void ABlasterCasterGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>();

	int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,
		600.f,
		FColor::Green,
		FString::Printf(TEXT("Number of players: %d"), NumOfPlayers - 1));
	}

	if(PlayerState)
	{
		FString PlayerName = PlayerState->GetPlayerName();
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 600.f, FColor::Blue, FString::Printf(TEXT("Player %s has joined!"), *PlayerName));
		}
	}
}
