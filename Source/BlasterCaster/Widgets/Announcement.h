// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Announcement.generated.h"

/**
 * 
 */
UCLASS()
class BLASTERCASTER_API UAnnouncement : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CountdownTime;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StartingText;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* InfoText;
};
