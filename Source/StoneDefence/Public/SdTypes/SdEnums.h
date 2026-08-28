#pragma once

/* Common Types */
UENUM(BlueprintType)
enum class ESdTextJustify : uint8
{
	Left UMETA(DisplayName = "Left"),
	Center UMETA(DisplayName = "Center"),
	Right UMETA(DisplayName = "Right")
};

UENUM(BlueprintType)
enum class ESdInputMode : uint8
{
	GameOnly,
	UIOnly,
	GameAndUI
};

/********/