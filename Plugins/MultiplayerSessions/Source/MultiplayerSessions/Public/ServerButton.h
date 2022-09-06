// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "ServerButton.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UServerButton : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* LeaderName;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* MapName;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* AmountPlayers;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Ping;

	UPROPERTY(meta=(BindWidget))
	class UButton* ServerButton;

	FOnlineSessionSearchResult SessionAssigned;

	UFUNCTION()
	void OnPressedButton();
	
	void Init(FOnlineSessionSearchResult SessionFound);
};
