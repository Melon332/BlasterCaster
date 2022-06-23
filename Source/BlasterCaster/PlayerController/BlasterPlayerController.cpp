// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "BlasterCaster/Character/BlasterCharacter.h"
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

		bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->EliminatedText;
		if(bHUDValid)
		{
			DeactivateEliminatedText();
		}
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

void ABlasterPlayerController::SetHUDDefeat(int32 Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
	BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->DefeatAmount;
	
	if(bHUDValid)
	{
		FString DefeatText = FString::Printf(TEXT("%d"), Score);
		BlasterHUD->CharacterOverlay->DefeatAmount->SetText(FText::FromString(DefeatText));
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
