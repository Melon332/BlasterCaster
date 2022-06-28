// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ALobbyHUD : public AHUD
{
	GENERATED_BODY()
public:
	void AddLobbyDisplay();
	void AddAnnouncementTimerToDisplay();
	
	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	TSubclassOf<class UUserWidget> LobbyCharacterOverlayClass;

	UPROPERTY()
	class ULobbyCharacterOverlay* LobbyCharacterOverlay;

	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	TSubclassOf<UUserWidget> AnnouncementOverlayClass;

	UPROPERTY()
	class UAnnouncement* AnnouncementOverlay;
protected:
private:
};
