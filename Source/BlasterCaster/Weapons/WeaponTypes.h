#pragma once

#define TRACE_LENGTH 80000
#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle = 0 UMETA(DisplayName = "Assault Rifle"),
	EWT_Pistol = 1 UMETA(DisplayName = "Pistol"),
	EWT_RocketLauncher = 2 UMETA(DisplayName = "Rocket Launcher"),
	EWT_SMG = 3 UMETA(DisplayName = "SMG"),
	EWT_Shotgun = 5 UMETA(DisplayName = "Shotgun"),
	EWT_AWP = 6 UMETA(DisplayName = "Sniper Rifle"),
	EWT_GrenadeLauncher = 7 UMETA(DisplayName = "Grenade Launcher"),
	EWT_MAX = 8 UMETA(DisplayName = "MAX"),
};
