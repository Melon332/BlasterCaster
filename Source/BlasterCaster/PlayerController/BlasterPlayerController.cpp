// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"

#include "BlasterCaster/Character/BlasterCharacter.h"
#include "BlasterCaster/Components/CombatComponent.h"
#include "BlasterCaster/PlayerState/BlasterPlayerState.h"
#include "BlasterCaster/Widgets/BlasterHUD.h"
#include "BlasterCaster/Widgets/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Net/UnrealNetwork.h"
#include "BlasterCaster/GameMode/BlasterGameMode.h"
#include "BlasterCaster/Widgets/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "BlasterCaster/GameStates/BlasterGameState.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());

	ServerCheckMatchState();
}

void ABlasterPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	SetHUDTime();

	CheckTimeSync(DeltaSeconds);

	PollInit();
}

void ABlasterPlayerController::CheckTimeSync(float DeltaSeconds)
{
	TimeSyncRunningTime += DeltaSeconds;

	if(IsLocalController() && TimeSyncRunningTime >= TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0;
	}
}

void ABlasterPlayerController::PollInit()
{
	if(CharacterOverlay == nullptr)
	{
		if(BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if(CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeat(HUDDefeats);
			}
		}
	}
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

void ABlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0;

	if(MatchState == ::MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if(MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if(MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	/*
	if(HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if(BlasterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCountDownTime() + LevelStartingTime);
		}
	}
	*/
	if(CountdownInt != SecondsLeft)
	{
		if(MatchState == ::MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDWarmupCountdown(TimeLeft);
		}
		if(MatchState == ::MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	
	CountdownInt = SecondsLeft;
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float ClientTime)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(ClientTime, ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;

	float CurrentServerTime = TimeServerReceivedClientRequest + RoundTripTime / 2;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
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
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Deaths;
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

void ABlasterPlayerController::SetHUDWeaponName(FString WeaponName, FString WeaponType)
{
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->WeaponName;
	if(bHUDValid)
	{
		FString WeaponText = FString::Printf(TEXT("%s - %s"), *WeaponName, *WeaponType);
		BlasterHUD->CharacterOverlay->WeaponName->SetText(FText::FromString(WeaponText));
	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->MatchCountdownText;
	if(bHUDValid)
	{
		if(CountdownTime < 0.f)
		{
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"),Minutes, Seconds);
		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDWarmupCountdown(float WarmupCountdown)
{
	bool bHUDValid = BlasterHUD && BlasterHUD->AnnouncementOverlay && BlasterHUD->AnnouncementOverlay->CountdownTime;
	if(bHUDValid)
	{
		if(WarmupCountdown < 0.f)
		{
			BlasterHUD->AnnouncementOverlay->CountdownTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(WarmupCountdown / 60);
		int32 Seconds = WarmupCountdown - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"),Minutes, Seconds);
		BlasterHUD->AnnouncementOverlay->CountdownTime->SetText(FText::FromString(CountdownText));
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

void ABlasterPlayerController::HandleMatchHasStarted()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD)
	{
		BlasterHUD->AddCharacterOverlay();
		if(BlasterHUD->AnnouncementOverlay)
		{
			BlasterHUD->AnnouncementOverlay->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ABlasterPlayerController::HandleCooldownHasStarted()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if(BlasterHUD)
	{
		BlasterHUD->CharacterOverlay->RemoveFromParent();
		bool bHUDValid = BlasterHUD->AnnouncementOverlay &&
			BlasterHUD->AnnouncementOverlay->StartingText &&
				BlasterHUD->AnnouncementOverlay->InfoText;
		if(bHUDValid)
		{
			BlasterHUD->AnnouncementOverlay->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In: ");
			BlasterHUD->AnnouncementOverlay->StartingText->SetText(FText::FromString(AnnouncementText));

			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
			if(ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this)))
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopBlasterPlayers;
				FString TopPlayerText;
				
				if(TopPlayers.Num() == 0)
				{
					TopPlayerText = FString(TEXT("You all suck balls, no one even scored anything"));
				}
				else if(TopPlayers.Num() == 1 && TopPlayers[0] == BlasterPlayerState)
				{
					TopPlayerText = FString(TEXT("You are the winner!!!"));
				}
				else if(TopPlayers.Num() == 1)
				{
					TopPlayerText = FString::Printf(TEXT("The winner is: \n%s"),*TopPlayers[0]->GetPlayerName());
				}
				else if(TopPlayers.Num() > 1)
				{
					TopPlayerText = FString(TEXT("TIE\n"));
					for(ABlasterPlayerState* TiedPlayer : TopPlayers)
					{
						TopPlayerText.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetName()));
					}
				}
				
				BlasterHUD->AnnouncementOverlay->InfoText->SetText(FText::FromString(TopPlayerText));
			}
		}
	}
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if(BlasterCharacter && BlasterCharacter->GetCombatComponent())
	{
		BlasterCharacter->bDisableGameplay = true;
		BlasterCharacter->GetCombatComponent()->FireButtonPressed(false);
	}
}

void ABlasterPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match,
                                                                float StartingTime, float Cooldown)
{
	MatchTime = Match;
	WarmupTime = Warmup;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	CooldownTime = Cooldown;
	OnMatchStateSet(MatchState);
	if(BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncementOverlay();
	}
}


void ABlasterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	
	if(MatchState == ::MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if(MatchState == ::MatchState::Cooldown)
	{
		HandleCooldownHasStarted();
	}
}


void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	if(ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		MatchTime = GameMode->MatchTime;
		WarmupTime = GameMode->WarmupTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		CooldownTime = GameMode->CooldownDuration;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, LevelStartingTime, CooldownTime);
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if(MatchState == ::MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if(MatchState == MatchState::Cooldown)
	{
		HandleCooldownHasStarted();
	}
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState);
}

float ABlasterPlayerController::GetServerTime()
{
	if(HasAuthority()) return GetWorld()->GetTimeSeconds();
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if(IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

