// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"

#include "BlasterCaster/PlayerController/LobbyPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void ALobbyGameState::IncreasePlayers(int32 MaximumPlayers)
{
	Players++;
	MaxPlayers = MaximumPlayers;
	if(ALobbyPlayerController* PlayerController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PlayerController->UpdatePlayerAmount();
	}
}

void ALobbyGameState::DecreasePlayers()
{
	Players--;
	if(ALobbyPlayerController* PlayerController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PlayerController->UpdatePlayerAmount();
	}
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGameState, Players);
	DOREPLIFETIME(ALobbyGameState, MaxPlayers);
}

void ALobbyGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameState::OnRep_PlayersAdded()
{
	if(ALobbyPlayerController* PlayerController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PlayerController->UpdatePlayerAmount();
	}
}
