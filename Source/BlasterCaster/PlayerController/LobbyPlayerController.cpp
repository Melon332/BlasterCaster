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
#include "Kismet/GameplayStatics.h"
#include "BlasterCaster/GameMode/LobbyGameMode.h"
#include "BlasterCaster/Widgets/Announcement.h"

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
	ServerCheckMatchState();
}

void ALobbyPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(CurrentMatchState == MatchState::AllPlayersReady)
	{
		CurrentTimer -= DeltaSeconds;
		UpdateAnnouncementTimer(CurrentTimer);
	}
}

void ALobbyPlayerController::OnRep_MatchState()
{
	if(CurrentMatchState == MatchState::AllPlayersReady)
	{
		HandleAllPlayersJoined();
	}
}

void ALobbyPlayerController::HandleAllPlayersJoined()
{
	LobbyHUD = LobbyHUD == nullptr ? Cast<ALobbyHUD>(GetHUD()) : LobbyHUD;
	if(LobbyHUD)
	{
		if(LobbyHUD->LobbyCharacterOverlay)
		{
			LobbyHUD->LobbyCharacterOverlay->RemoveFromParent();
		}
		CurrentTimer = LevelStartingTimer;
		LobbyHUD->AddAnnouncementTimerToDisplay();
	}
}

void ALobbyPlayerController::UpdateAnnouncementTimer(float Timer)
{
	bool bHUDValid = LobbyHUD && LobbyHUD->AnnouncementOverlay && LobbyHUD->AnnouncementOverlay->CountdownTime;
	if(bHUDValid)
	{
		if(Timer < 0.f)
		{
			LobbyHUD->AnnouncementOverlay->CountdownTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(Timer / 60);
		int32 Seconds = Timer - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"),Minutes, Seconds);
		LobbyHUD->AnnouncementOverlay->CountdownTime->SetText(FText::FromString(CountdownText));
	}
}

void ALobbyPlayerController::ClientGetMatchStateInformation_Implementation(FName MatchState, float CurrentTime, float StartingTime)
{
	CurrentMatchState = MatchState;
	CurrentTimer = CurrentTime;
	LevelStartingTimer = StartingTime;
	OnMatchStateSet(CurrentMatchState);
	GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, FString::Printf(TEXT("CLIENT: Match state is: %s"), *CurrentMatchState.ToString()));
}

void ALobbyPlayerController::ServerCheckMatchState_Implementation()
{
	if(ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		CurrentMatchState = LobbyGameMode->GetMatchState();
		CurrentTimer = LobbyGameMode->CurrentCountdownTime;
		LevelStartingTimer = LobbyGameMode->LevelStartingTimer;
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, FString::Printf(TEXT("Match state is: %s"), *CurrentMatchState.ToString()));
		ClientGetMatchStateInformation(CurrentMatchState, CurrentTimer, LevelStartingTimer);
	}
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
		if(LobbyHUD->LobbyCharacterOverlay)
		{
			LobbyHUD->LobbyCharacterOverlay->RemoveFromParent();
		}
		if(LobbyHUD->AnnouncementOverlay)
		{
			LobbyHUD->AnnouncementOverlay->RemoveFromParent();
		}
	}
}

void ALobbyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerController, CurrentMatchState);
}

void ALobbyPlayerController::OnMatchStateSet(FName MatchState)
{
	CurrentMatchState = MatchState;
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, FString::Printf(TEXT("Match state set to: %s"), *FString(MatchState.ToString())));
	}
	if(MatchState == MatchState::AllPlayersReady)
	{
		HandleAllPlayersJoined();
	}
}
