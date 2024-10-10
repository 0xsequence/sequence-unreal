// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "BE_Enums.generated.h"

UENUM(BlueprintType)
enum ESocialSigninType
{
	Google UMETA(DisplayName = "Google"),
	FaceBook UMETA(DisplayName = "FaceBook"),
	Discord UMETA(DisplayName = "Discord"),
	Apple UMETA(DisplayName = "Apple"),
};