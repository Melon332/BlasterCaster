// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BlasterPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void OnRep_Score() override;

	//Replication for defeats
	UFUNCTION()
	virtual void OnRep_OnDefeats();
	
	void AddToScore(float ScoreAmount);
	void UpdateDefeatHUD();
	void AddToDefeats(int32 DefeatAmount);
protected:
	
private:
	UPROPERTY()
	class ABlasterCharacter* BlasterCharacter{nullptr};
	UPROPERTY()
	class ABlasterPlayerController* BlasterPlayerController{nullptr};

	UPROPERTY(ReplicatedUsing=OnRep_OnDefeats)
	int32 Defeats{0};
};
