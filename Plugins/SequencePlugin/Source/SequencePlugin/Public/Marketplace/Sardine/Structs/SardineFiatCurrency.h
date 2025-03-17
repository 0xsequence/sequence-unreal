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

	void Setup(FJsonObject& json_in)
	{
		json_in.TryGetStringField(TEXT("currencyCode"), CurrencyCode);
		json_in.TryGetStringField(TEXT("name"), Name);
		json_in.TryGetStringField(TEXT("currencySymbol"), CurrencySymbol);

		for(TSharedPtr<FJsonValue> Option : json_in.GetArrayField(TEXT("paymentOptions")))
		{
			FSardinePaymentOption PaymentOption;
			PaymentOption.Setup(*Option->AsObject());
			PaymentOptions.Add(PaymentOption);
		}

		json_in.TryGetStringArrayField(TEXT("supportedCountries"), SupportedCountries);
	}
};