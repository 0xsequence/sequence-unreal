#pragma once

#include "CoreMinimal.h"
#include "SardinePaymentMethod.generated.h"

UENUM(BlueprintType)
enum ESardinePaymentMethod
{
	Us_Debit  UMETA(DisplayName = "US_DEBIT"),
	Us_Credit  UMETA(DisplayName = "US_CREDIT"),
	International_Debit  UMETA(DisplayName = "INTERNATIONAL_DEBIT"),
	International_Credit  UMETA(DisplayName = "INTERNATIONAL_CREDIT"),
	Ach UMETA(DisplayName = "ACH"),
};