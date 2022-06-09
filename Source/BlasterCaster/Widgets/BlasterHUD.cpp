// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2d ViewportSize;
	if(GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2d Center(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		if(HUDPackage.CrosshairCenter)
		{
			DrawCrosshair(HUDPackage.CrosshairCenter, Center);
		}
		if(HUDPackage.CrosshairLeft)
		{
			DrawCrosshair(HUDPackage.CrosshairLeft,Center);
		}
		if(HUDPackage.CrosshairRight)
		{
			DrawCrosshair(HUDPackage.CrosshairRight,Center);
		}
		if(HUDPackage.CrosshairTop)
		{
			DrawCrosshair(HUDPackage.CrosshairTop,Center);
		}
		if(HUDPackage.CrosshairBottom)
		{
			DrawCrosshair(HUDPackage.CrosshairBottom,Center);
		}
	}

	
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Crosshair, FVector2d ViewportCenter)
{
	const float TextureWidth = Crosshair->GetSizeX();
	const float TextureHeight = Crosshair->GetSizeY();

	const FVector2d TextureDrawPoint = FVector2d(ViewportCenter.X - (TextureWidth / 2) / 2, ViewportCenter.Y - (TextureHeight / 2) / 2);

	DrawTexture(Crosshair, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f);
}
