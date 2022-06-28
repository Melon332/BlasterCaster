// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "BlasterCaster/GameStates/LobbyGameState.h"
#include "BlasterCaster/PlayerController/LobbyPlayerController.h"
#include "BlasterCaster/Widgets/LobbyCharacterOverlay.h"
#include "BlasterCaster/Widgets/LobbyHUD.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

namespace MatchState
{
	const FName AllPlayersReady = FName("AllPlayersReady");
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentCountdownTime = LevelStartingTimer;
}

void ALobbyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(MatchState == MatchState::AllPlayersReady)
	{
		CurrentCountdownTime -= DeltaSeconds;
		if(CurrentCountdownTime <= 0.f)
		{
			LoadLevel(FString(TEXT("BlasterMap")));
		}
	}
}

void ALobbyGameMode::LoadLevel(FString LevelAddress)
{
	if(UWorld* World = GetWorld())
	{
		FString LevelToLoad = FString::Printf(TEXT("/Game/Maps/%s?listen"), *LevelAddress);
		bUseSeamlessTravel = true;
		World->ServerTravel(LevelToLoad);
	}
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();

	if(NumOfPlayers == MaxPlayers)
	{
		SetMatchState(MatchState::AllPlayersReady);
	}

	if(ALobbyGameState* LobbyGameState = Cast<ALobbyGameState>(GameState))
	{
		LobbyGameState->IncreasePlayers(MaxPlayers);
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

void ALobbyGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if(ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(*It))
		{
			LobbyPlayerController->OnMatchStateSet(MatchState);
		}
	}
}

void ALobbyGameMode::ForceStart()
{
	SetMatchState(MatchState::AllPlayersReady);
}