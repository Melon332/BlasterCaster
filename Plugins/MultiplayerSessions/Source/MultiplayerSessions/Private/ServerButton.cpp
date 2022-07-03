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

void UServerButton::Init()
{
	if(ServerButton)
	{
		ServerButton->OnClicked.AddDynamic(this, &ThisClass::OnPressedButton);
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, TEXT("Subscribed!"));
		}
	}
}