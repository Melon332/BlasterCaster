// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "BlasterCaster/Character/BlasterCharacter.h"
#include "BlasterCaster/PlayerState/BlasterPlayerState.h"
#include "BlasterCaster/Widgets/BlasterHUD.h"
#include "BlasterCaster/Widgets/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
}


void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn))
	{
		SetHUDHealth(BlasterCharacter->GetCurrentHealth(), BlasterCharacter->GetMaxHealth());

		DeactivateEliminatedText();
		BlasterCharacter->SetDiedFromFalling(false);
	}
}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
			BlasterHUD->CharacterOverlay->HealthBar &&
				BlasterHUD->CharacterOverlay->HealthText;
	
	if(bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
	BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;
	
	if(bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	
}

void ABlasterPlayerController::SetHUDDefeat(int32 Deaths)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->DefeatAmount;
	
	if(bHUDValid)
	{
		UE_LOG(LogTemp,Warning, TEXT("The HUD is valid"));
		FString DefeatText = FString::Printf(TEXT("%d"), Deaths);
		BlasterHUD->CharacterOverlay->DefeatAmount->SetText(FText::FromString(DefeatText));
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	bool bHUDValid = BlasterHUD &&
	BlasterHUD->CharacterOverlay &&
	BlasterHUD->CharacterOverlay->WeaponAmmoCount;
	
	if(bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoCount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	bool bHUDValid = BlasterHUD &&
	BlasterHUD->CharacterOverlay &&
	BlasterHUD->CharacterOverlay->CarriedAmmoAmount;
	
	if(bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerController::SetHUDWeaponName(FString WeaponName)
{
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->WeaponName;
	if(bHUDValid)
	{
		BlasterHUD->CharacterOverlay->WeaponName->SetText(FText::FromString(WeaponName));
	}
}

void ABlasterPlayerController::ActivateEliminatedText()
{
	bool bHUDValid = BlasterHUD &&
	BlasterHUD->CharacterOverlay &&
	BlasterHUD->CharacterOverlay->EliminatedText;
	
	if(bHUDValid)
	{
		BlasterHUD->CharacterOverlay->EliminatedText->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABlasterPlayerController::DeactivateEliminatedText()
{
	bool bHUDValid = BlasterHUD &&
	BlasterHUD->CharacterOverlay &&
	BlasterHUD->CharacterOverlay->EliminatedText;
	
	if(bHUDValid)
	{
		BlasterHUD->CharacterOverlay->EliminatedText->SetVisibility(ESlateVisibility::Hidden);
	}
}

