// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "SardineEnabledToken.h"
#include "SardinePaymentType.h"
#include "SardineQuote.h"
#include "Util/JsonBuilder.h"
#include "SardineQuoteType.h"
#include "UObject/Object.h"
#include "SardineEnabledTokensResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineEnabledTokensResponse
{
	GENERATED_USTRUCT_BODY()

public:
	TArray<FSardineEnabledToken> Tokens;

	bool customConstructor = false;

	void construct(FJsonObject& json_in) {}
	void Setup(FJsonObject& json_in)
	{
		for(TSharedPtr<FJsonValue> json : json_in.GetArrayField(TEXT("tokens")))
		{
			FSardineEnabledToken Token;
			Token.Setup(*json->AsObject());
			Tokens.Add(Token);
		}
	}
};