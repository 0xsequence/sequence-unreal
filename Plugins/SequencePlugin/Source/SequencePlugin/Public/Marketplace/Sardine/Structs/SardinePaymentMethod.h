#pragma once

#include "CoreMinimal.h"
#include "SardinePaymentMethod.generated.h"

UENUM(BlueprintType)
enum ESardinePaymentMethod
{
	PaymentMethod_Us_Debit  UMETA(DisplayName = "US_DEBIT"),
	PaymentMethod_Us_Credit  UMETA(DisplayName = "US_CREDIT"),
	PaymentMethod_International_Debit  UMETA(DisplayName = "INTERNATIONAL_DEBIT"),
	PaymentMethod_International_Credit  UMETA(DisplayName = "INTERNATIONAL_CREDIT"),
	PaymentMethod_Ach UMETA(DisplayName = "ACH"),
};