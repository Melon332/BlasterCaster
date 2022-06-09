#pragma once

UENUM(BlueprintType)
enum class ETurningState : uint8
{
	ETIP_LEFT UMETA(DisplayName = "Turn Left"),
	ETIP_RIGHT UMETA(DisplayName = "Turn Right"),
	ETIP_NOTTURNING UMETA(DisplayName = "Not Turning"),
	ETIP_MAX UMETA(DisplayName = "Default Max"),
};