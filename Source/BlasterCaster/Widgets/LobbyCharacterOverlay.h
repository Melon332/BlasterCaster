// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyCharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ULobbyCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* NumberPlayerCount;
};
