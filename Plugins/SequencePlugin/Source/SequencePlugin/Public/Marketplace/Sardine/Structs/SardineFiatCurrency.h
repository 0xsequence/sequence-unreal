#pragma once

#include "CoreMinimal.h"
#include "SardinePaymentOption.h"
#include "SardineFiatCurrency.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineFiatCurrency
{
	GENERATED_USTRUCT_BODY()

public:
	FString CurrencyCode;
	FString Name;
	FString CurrencySymbol;
	TArray<FSardinePaymentOption> PaymentOptions;
	TArray<FString> SupportedCountries;
};