// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyHUD.h"

#include "Blueprint/UserWidget.h"
#include "BlasterCaster/Widgets/LobbyCharacterOverlay.h"
#include "Components/TextBlock.h"

void ALobbyHUD::AddLobbyDisplay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && LobbyCharacterOverlayClass)
	{
		LobbyCharacterOverlay = CreateWidget<ULobbyCharacterOverlay>(PlayerController, LobbyCharacterOverlayClass);
		LobbyCharacterOverlay->AddToViewport();
		if(!HasAuthority())
		{
			LobbyCharacterOverlay->NumberPlayerCount->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
