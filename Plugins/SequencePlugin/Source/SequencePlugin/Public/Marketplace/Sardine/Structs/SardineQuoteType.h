#pragma once

#include "CoreMinimal.h"
#include "SardineQuoteType.generated.h"

UENUM(BlueprintType)
enum ESardineQuoteType
{
	Buy   UMETA(DisplayName = "BUY"),
	Sell UMETA(DisplayName = "SELL"),
};