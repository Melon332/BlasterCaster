// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerButton.h"
#include "MultiplayerSessionSubsystem.h"
#include "Components/Button.h"

void UServerButton::OnPressedButton()
{
	if(const UGameInstance* GameInstance = GetGameInstance())
	{
		auto MultiplayerSessionSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionSubsystem>();
		if(MultiplayerSessionSubsystem)
		{
			MultiplayerSessionSubsystem->JoinSession(SessionAssigned);
		}
	}
}

void UServerButton::Init(FOnlineSessionSearchResult SessionFound)
{
	if(ServerButton)
	{
		SessionAssigned = SessionFound;
		ServerButton->OnClicked.AddDynamic(this, &ThisClass::OnPressedButton);
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, TEXT("Subscribed!"));
		}
	}
}