// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
class UButton;
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfMaxConnections = 4, FString MatchType = FString(TEXT("FreeForAll")), FString LobbyPath = TEXT("/Game/ThirdPerson/Maps/Lobby"));
protected:
	virtual bool Initialize() override;
	
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	//Callbacks for the custom delegates on the multiplayer subsystem
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
private:
	UPROPERTY(meta=(BindWidget))
	UButton* JoinButton;
	
	UPROPERTY(meta=(BindWidget))
	UButton* HostButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

	//Subsystem to handle online session functionality
	class UMultiplayerSessionSubsystem* MultiplayerSessionSubsystem;

	int32 MaxPlayers{4};
	FString MatchType{TEXT("Mehmet_UE_FreeForAll")};
	FString PathToLobby{};
};
