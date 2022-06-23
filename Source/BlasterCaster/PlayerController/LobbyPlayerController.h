// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY()
	class ALobbyHUD* LobbyHUD;
	
	void UpdatePlayerAmount();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
protected:

	virtual void BeginPlay() override;
private:
};
