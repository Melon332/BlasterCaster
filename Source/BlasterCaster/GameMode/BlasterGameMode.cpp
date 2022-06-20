// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "BlasterCaster/Character/BlasterCharacter.h"
#include "BlasterCaster/PlayerController/BlasterPlayerController.h"
#include "GameFramework/PlayerState.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* EliminatedPlayer,
                                        ABlasterPlayerController* EliminatedPlayerController, ABlasterPlayerController* AttackerController)
{
	if(GEngine)
	{
		APlayerState* PlayerState = EliminatedPlayerController->GetPlayerState<APlayerState>();
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue ,FString::Printf(TEXT("The player eliminated was: %s"), *PlayerState->GetPlayerName()));
	}
}
