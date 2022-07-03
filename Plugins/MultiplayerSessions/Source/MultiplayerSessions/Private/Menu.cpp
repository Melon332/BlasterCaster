// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

#include "Components/Button.h"
#include "MultiplayerSessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "ServerButton.h"
#include "Components/TextBlock.h"

void UMenu::MenuSetup(int32 NumberOfMaxConnections, FString matchType, FString LobbyPath)
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	MaxPlayers = NumberOfMaxConnections;
	MatchType = matchType;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

	UWorld* World = GetWorld();
	if(World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if(PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
	if(const UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionSubsystem>();
	}

	if(MultiplayerSessionSubsystem)
	{
		//Dynamic callbacks
		MultiplayerSessionSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
		MultiplayerSessionSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);

		//Non dynamic callbacks
		MultiplayerSessionSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSession);
		MultiplayerSessionSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
	}
}

bool UMenu::Initialize()
{
	if(!Super::Initialize())
	{
		return false;
	}
	if(HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if(JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	if(ServerBrowserButton)
	{
		ServerBrowserButton->OnClicked.AddDynamic(this, &ThisClass::ActivateServerBrowser);
	}
	if(BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &ThisClass::ReturnToMainMenu);
	}
	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,15,FColor::Blue, TEXT("Succesfully created session!"));
		}
		
		if(UWorld* World = GetWorld())
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,15,FColor::Red, TEXT("Failed to create session!"));
		}
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if(!MultiplayerSessionSubsystem) return;

	if(ServerBrowserSearch)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Finished looking for games!"));
		}
		for(auto Result : SessionResults)
		{
			UServerButton* WidgetToSpawn = CreateWidget<UServerButton>(this, ServerButton);
			if(WidgetToSpawn)
			{
				FString Ping = FString::Printf(TEXT("%d ms"), Result.PingInMs);
				FString Players = FString::Printf(TEXT("%d/%d"), Result.Session.SessionSettings.NumPublicConnections - Result.Session.NumOpenPublicConnections, Result.Session.SessionSettings.NumPublicConnections);
				WidgetToSpawn->Ping->SetText(FText::FromString(Ping));
				WidgetToSpawn->LeaderName->SetText(FText::FromString(Result.Session.OwningUserName));
				WidgetToSpawn->MapName->SetText(FText::FromString("de_oogabooga2"));
				WidgetToSpawn->AmountPlayers->SetText(FText::FromString(Players));
				WidgetToSpawn->Init();
				WidgetToSpawn->SessionAssigned = Result;
			}

			if(ServerList)
			{
				ServerList->AddChildToVerticalBox(WidgetToSpawn);
			}
		}
		ServerBrowserSearch = false;
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, TEXT("Finding..."));
	
	for(auto result : SessionResults)
	{
		FString SettingsValue;
		result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if(SettingsValue == MatchType)
		{
			FString Username = result.Session.OwningUserName;
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString::Printf(TEXT("Session found Called: %s"), *Username));
			MultiplayerSessionSubsystem->JoinSession(result);
			return;
		}
	}
	if(!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid() && Result == EOnJoinSessionCompleteResult::Success)
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString::Printf(TEXT("Entering Lobby world...")));
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	if(Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
		switch (Result)
		{
		case EOnJoinSessionCompleteResult::UnknownError:
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Unknown error occured!")));
			break;
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Could not get the address!")));
			break;
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("The session did not exist!!!")));
			break;
		}
	}
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
	if(bWasSuccessful)
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Silver, FString::Printf(TEXT("The Session has successfully started!")));
		}
	}
	else
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, FString::Printf(TEXT("The Session has failed to start!")));
		}
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
	
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("Host Button Clicked!")));
	}
	if(MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->CreateSession(MaxPlayers, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if(ServerBrowserButton)
	{
		ServerBrowserButton->SetIsEnabled(false);
	}
	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("Join Button Clicked")));
	}
	if(MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown()
{
	if(WidgetController->ActiveWidgetIndex == 1)
	{
		WidgetController->SetActiveWidgetIndex(0);
	}
	RemoveFromParent();
	if(UWorld* World = GetWorld())
	{
		if(APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeGameOnly InputModeGameOnly;
			InputModeGameOnly.SetConsumeCaptureMouseDown(true);
			PlayerController->SetInputMode(InputModeGameOnly);
			PlayerController->SetShowMouseCursor(false);
		}
	}
	
}

void UMenu::ActivateServerBrowser()
{
	if(WidgetController)
	{
		WidgetController->SetActiveWidgetIndex(1);
		ServerBrowserSearch = true;
		MultiplayerSessionSubsystem->FindSessions(10000);
	}
}

void UMenu::ReturnToMainMenu()
{
	if(WidgetController)
	{
		WidgetController->SetActiveWidgetIndex(0);
	}
	if(ServerList)
	{
		ServerList->ClearChildren();
	}
}
