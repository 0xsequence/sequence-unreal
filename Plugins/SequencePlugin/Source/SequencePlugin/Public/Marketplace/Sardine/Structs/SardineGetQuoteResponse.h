// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "SardinePaymentType.h"
#include "SardineQuote.h"
#include "Util/JsonBuilder.h"
#include "SardineQuoteType.h"
#include "UObject/Object.h"
#include "SardineGetQuoteResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineGetQuoteResponse
{
	GENERATED_USTRUCT_BODY()

public:
	FSardineQuote Quote;

	bool customConstructor = false;

	void construct(FJsonObject& json_in) {}
	void Setup(FJsonObject& json_in)
	{
		const auto QuoteObject = json_in.GetObjectField(TEXT("quote"));
		Quote.Setup(*QuoteObject);
	}
};