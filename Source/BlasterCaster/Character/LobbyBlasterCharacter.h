// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterCharacter.h"
#include "LobbyBlasterCharacter.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ALobbyBlasterCharacter : public ABlasterCharacter
{
	GENERATED_BODY()
public:
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
private:
	void ForceStartGame();
};
