// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

#include "CharacterOverlay.h"
#include "Blueprint/UserWidget.h"
#include "Announcement.h"


void ABlasterHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ABlasterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void ABlasterHUD::AddAnnouncementOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && AnnouncementOverlayClass)
	{
		AnnouncementOverlay = CreateWidget<UAnnouncement>(PlayerController, AnnouncementOverlayClass);
		AnnouncementOverlay->AddToViewport();
	}
}

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2d ViewportSize;
	if(GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2d Center(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		FVector2D Spread;
		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;
		if(HUDPackage.CrosshairCenter)
		{
			DrawCrosshair(HUDPackage.CrosshairCenter, Center, FVector2D(), HUDPackage.EnemyColor);
		}
		if(HUDPackage.CrosshairLeft)
		{
			Spread = FVector2D(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairLeft,Center, Spread, HUDPackage.EnemyColor);
		}
		if(HUDPackage.CrosshairRight)
		{
			Spread = FVector2D(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairRight,Center, Spread, HUDPackage.EnemyColor);
		}
		if(HUDPackage.CrosshairTop)
		{
			Spread = FVector2D(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairTop,Center, Spread, HUDPackage.EnemyColor);
		}
		if(HUDPackage.CrosshairBottom)
		{
			Spread = FVector2D(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairBottom,Center, Spread, HUDPackage.EnemyColor);
		}
	}

	
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Crosshair, FVector2d ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Crosshair->GetSizeX();
	const float TextureHeight = Crosshair->GetSizeY();

	const FVector2d TextureDrawPoint = FVector2d(ViewportCenter.X - (TextureWidth / 2) / 2 + Spread.X, ViewportCenter.Y - (TextureHeight / 2) / 2 + Spread.Y);

	DrawTexture(Crosshair, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, CrosshairColor);
}
