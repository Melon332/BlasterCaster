#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle = 0 UMETA(DisplayName = "Assault Rifle"),
	EWT_Pistol = 1 UMETA(DisplayName = "Pistol"),
	EWT_MAX = 2 UMETA(DisplayName = "MAX"),
};
