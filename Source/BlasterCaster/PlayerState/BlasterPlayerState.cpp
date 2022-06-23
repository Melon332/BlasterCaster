// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"

#include "BlasterCaster/Character/BlasterCharacter.h"
#include "BlasterCaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if(BlasterCharacter)
	{
		BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->Controller) : BlasterPlayerController;
		if(BlasterPlayerController)
		{
			BlasterPlayerController->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);

	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if(BlasterCharacter && BlasterCharacter->Controller)
	{
		BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->Controller) : BlasterPlayerController;
		if(BlasterPlayerController)
		{
			BlasterPlayerController->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatAmount)
{
	Defeats += DefeatAmount;

	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if(BlasterCharacter && BlasterCharacter->Controller)
	{
		BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->Controller) : BlasterPlayerController;
		if(BlasterPlayerController)
		{
			BlasterPlayerController->SetHUDDefeat(Defeats);
			if(Defeats != 0)
			{
				BlasterPlayerController->ActivateEliminatedText();
			}
		}
	}
}

void ABlasterPlayerState::ShowDefeatText()
{
	if(Defeats != 0)
	{
		BlasterPlayerController->ActivateEliminatedText();
	}
	
}

void ABlasterPlayerState::HideDefeatText()
{
	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if(BlasterCharacter && BlasterCharacter->Controller)
	{
		BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->Controller) : BlasterPlayerController;
		if(BlasterPlayerController)
		{
			BlasterPlayerController->DeactivateEliminatedText();
		}
	}
}

void ABlasterPlayerState::OnRep_OnDefeats()
{
	BlasterCharacter = BlasterCharacter == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : BlasterCharacter;
	if(BlasterCharacter)
	{
		BlasterPlayerController = BlasterPlayerController == nullptr ? Cast<ABlasterPlayerController>(BlasterCharacter->Controller) : BlasterPlayerController;
		if(BlasterPlayerController)
		{
			BlasterPlayerController->SetHUDDefeat(Defeats);
			if(Defeats != 0)
			{
				BlasterPlayerController->ActivateEliminatedText();
			}
		}
	}
}
