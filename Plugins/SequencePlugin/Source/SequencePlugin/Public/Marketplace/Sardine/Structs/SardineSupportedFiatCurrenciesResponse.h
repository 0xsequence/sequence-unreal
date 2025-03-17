// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "SardineEnabledToken.h"
#include "SardineFiatCurrency.h"
#include "SardinePaymentType.h"
#include "SardineQuote.h"
#include "Util/JsonBuilder.h"
#include "SardineQuoteType.h"
#include "SardineSupportedToken.h"
#include "UObject/Object.h"
#include "SardineSupportedFiatCurrenciesResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineSupportedFiatCurrenciesResponse
{
	GENERATED_USTRUCT_BODY()

public:
	TArray<FSardineFiatCurrency> Tokens;

	bool customConstructor = false;

	void construct(FJsonObject& json_in) {}
	void Setup(FJsonObject& json_in)
	{
		for(TSharedPtr<FJsonValue> json : json_in.GetArrayField(TEXT("tokens")))
		{
			FSardineFiatCurrency Currency;
			Currency.Setup(*json->AsObject());
			Tokens.Add(Currency);
		}
	}
};