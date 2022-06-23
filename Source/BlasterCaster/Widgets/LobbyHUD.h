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
	
	UPROPERTY(EditDefaultsOnly, Category="Lobby")
	TSubclassOf<class UUserWidget> LobbyCharacterOverlayClass;

	UPROPERTY()
	class ULobbyCharacterOverlay* LobbyCharacterOverlay;
protected:
private:
};
