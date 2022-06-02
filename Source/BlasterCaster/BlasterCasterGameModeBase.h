// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BlasterCasterGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ABlasterCasterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void Logout(AController* Exiting) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
