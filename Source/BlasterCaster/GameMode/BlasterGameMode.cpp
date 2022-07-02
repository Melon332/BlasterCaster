// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "BlasterCaster/Character/BlasterCharacter.h"
#include "BlasterCaster/PlayerController/BlasterPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "BlasterCaster/PlayerState/BlasterPlayerState.h"
#include "BlasterCaster/GameStates/BlasterGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if(MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if(MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownDuration + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayerController = Cast<ABlasterPlayerController>(*It);
		if(BlasterPlayerController)
		{
			BlasterPlayerController->OnMatchStateSet(MatchState);
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* EliminatedPlayer,
                                        ABlasterPlayerController* EliminatedPlayerController, ABlasterPlayerController* AttackerController)
{
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = EliminatedPlayerController ? Cast<ABlasterPlayerState>(EliminatedPlayerController->PlayerState) : nullptr;

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();

	if(AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
		if(BlasterGameState)
		{
			BlasterGameState->UpdateTopScore(AttackerPlayerState);
		}
	}
	
	if(VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
		EliminatedPlayerController->SetLastDefeatName(AttackerController->PlayerState->GetPlayerName());
	}
	
	if(EliminatedPlayer)
	{
		EliminatedPlayer->Eliminated();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* EliminatedPlayer, AController* EliminatedController)
{
	if(EliminatedPlayer)
	{
		EliminatedPlayer->Reset();
		EliminatedPlayer->Destroy();
	}
	if(EliminatedController)
	{
		RestartPlayerAtPlayerStart(EliminatedController, FindRandomSpawnPosition(EliminatedPlayer));
	}
}

AActor* ABlasterGameMode::FindRandomSpawnPosition(AActor* EliminatedPlayer)
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(),PlayerStarts);
	int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
	/*
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(this, ABlasterCharacter::StaticClass(), Players);
	float FurthestDist{0};
	AActor* FurthestStartPos{nullptr};
	for(AActor* StartPos : PlayerStarts)
	{
		for(AActor* Player : Players)
		{
			if(Player == EliminatedPlayer) continue;
			float Dist = Player->GetDistanceTo(StartPos);
			GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Blue, FString::Printf(TEXT("Distance is: %f"), Dist));
			if(Dist >= DistanceBetweenPlayers)
			{
				FurthestDist += Dist;
				if(FurthestDist >= Dist)
				{
					FurthestStartPos = StartPos;
					FurthestDist = Dist;
				}
			}
		}
	}
	
	
	if(!FurthestStartPos)
	{
		return PlayerStarts[Selection];
	}
	*/
	return PlayerStarts[Selection];
}
