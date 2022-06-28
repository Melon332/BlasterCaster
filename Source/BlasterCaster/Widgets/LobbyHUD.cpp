// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyHUD.h"

#include "Blueprint/UserWidget.h"
#include "BlasterCaster/Widgets/LobbyCharacterOverlay.h"
#include "Announcement.h"
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

void ALobbyHUD::AddAnnouncementTimerToDisplay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && AnnouncementOverlayClass)
	{
		AnnouncementOverlay = CreateWidget<UAnnouncement>(PlayerController, AnnouncementOverlayClass);
		AnnouncementOverlay->AddToViewport();
		if(AnnouncementOverlay->InfoText)
		{
			AnnouncementOverlay->InfoText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
