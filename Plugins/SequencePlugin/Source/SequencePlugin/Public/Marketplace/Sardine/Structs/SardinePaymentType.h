#pragma once

#include "CoreMinimal.h"
#include "SardinePaymentType.generated.h"

UENUM(BlueprintType)
enum ESardinePaymentType
{
	Ach   UMETA(DisplayName = "ACH"),
	Debit UMETA(DisplayName = "DEBIT"),
	Credit UMETA(DisplayName = "CREDIT"),
	Us_Debit UMETA(DisplayName = "US_DEBIT"),
	International_Debit UMETA(DisplayName = "INTERNATIONAL_DEBIT"),
	International_Credit UMETA(DisplayName = "INTERNATIONAL_CREDIT"),
};