#pragma once

#define TRACE_LENGTH 80000
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle = 0 UMETA(DisplayName = "Assault Rifle"),
	EWT_Pistol = 1 UMETA(DisplayName = "Pistol"),
	EWT_RocketLauncher = 2 UMETA(DisplayName = "Rocket Launcher"),
	EWT_SMG = 3 UMETA(DisplayName = "SMG"),
	EWT_Shotgun = 5 UMETA(DisplayName = "Shotgun"),
	EWT_AWP = 6 UMETA(DisplayName = "Sniper Rifle"),
	EWT_MAX = 7 UMETA(DisplayName = "MAX"),
};
