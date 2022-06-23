// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "BlasterCaster/GameStates/LobbyGameState.h"
#include "BlasterCaster/PlayerController/LobbyPlayerController.h"
#include "BlasterCaster/Widgets/LobbyCharacterOverlay.h"
#include "BlasterCaster/Widgets/LobbyHUD.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();

	if(ALobbyGameState* LobbyGameState = Cast<ALobbyGameState>(GameState))
	{
		LobbyGameState->IncreasePlayers(MaxPlayers);
	}
	if(NumOfPlayers == MaxPlayers)
	{
		if(UWorld* World = GetWorld())
		{
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	if(ALobbyGameState* LobbyGameState = Cast<ALobbyGameState>(GameState))
	{
		LobbyGameState->DecreasePlayers();
	}
}

void ALobbyGameMode::ForceStart()
{
	if(UWorld* World = GetWorld())
	{
		bUseSeamlessTravel = true;
		World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
	}
}

