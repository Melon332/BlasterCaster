// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyHUD.h"

#include "Blueprint/UserWidget.h"
#include "BlasterCaster/Widgets/LobbyCharacterOverlay.h"

void ALobbyHUD::AddLobbyDisplay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && LobbyCharacterOverlayClass)
	{
		LobbyCharacterOverlay = CreateWidget<ULobbyCharacterOverlay>(PlayerController, LobbyCharacterOverlayClass);
		LobbyCharacterOverlay->AddToViewport();
	}
}