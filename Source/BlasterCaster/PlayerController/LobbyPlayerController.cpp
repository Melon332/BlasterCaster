// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"

#include "BlasterCaster/GameMode/LobbyGameMode.h"
#include "BlasterCaster/GameStates/LobbyGameState.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "BlasterCaster/Widgets/LobbyCharacterOverlay.h"
#include "Components/TextBlock.h"
#include "BlasterCaster/Widgets/LobbyHUD.h"
#include "GameFramework/GameStateBase.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	LobbyHUD = Cast<ALobbyHUD>(GetHUD());
	if(LobbyHUD)
	{
		LobbyHUD->AddLobbyDisplay();
		if(LobbyHUD->LobbyCharacterOverlay && LobbyHUD->LobbyCharacterOverlay->ServerEnterText)
		{
			if(!HasAuthority())
			{
				LobbyHUD->LobbyCharacterOverlay->ServerEnterText->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	UpdatePlayerAmount();
}

void ALobbyPlayerController::UpdatePlayerAmount()
{
	if(ALobbyGameState* LobbyGameState = Cast<ALobbyGameState>(GetWorld()->GetGameState()))
	{
		LobbyHUD = LobbyHUD == nullptr ? Cast<ALobbyHUD>(GetHUD()) : LobbyHUD;
		if(LobbyHUD)
		{
			if(LobbyHUD && LobbyHUD->LobbyCharacterOverlay && LobbyHUD->LobbyCharacterOverlay->NumberPlayerCount)
			{
				FString PlayerNumber = FString::Printf(TEXT("%d/%d"), LobbyGameState->Players, LobbyGameState->MaxPlayers);
				LobbyHUD->LobbyCharacterOverlay->NumberPlayerCount->SetText(FText::FromString(PlayerNumber));
			}
		}
	}
}

void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(LobbyHUD)
	{
		LobbyHUD->LobbyCharacterOverlay->RemoveFromParent();
	}
}
