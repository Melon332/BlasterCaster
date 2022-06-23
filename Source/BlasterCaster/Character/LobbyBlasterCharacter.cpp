// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyBlasterCharacter.h"

#include "BlasterCaster/GameMode/LobbyGameMode.h"

void ALobbyBlasterCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	//Super::FellOutOfWorld(dmgType);

	TeleportTo(FVector(0,0,100), FRotator(), false, false);
}

void ALobbyBlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("ForceStartGame"), IE_Pressed,this ,&ThisClass::ForceStartGame);
}

void ALobbyBlasterCharacter::ForceStartGame()
{
	if(HasAuthority())
	{
		if(ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode()))
		{
			LobbyGameMode->ForceStart();
		}
	}
}
