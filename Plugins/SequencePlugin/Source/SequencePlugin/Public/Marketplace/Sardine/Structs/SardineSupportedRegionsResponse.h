// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "SardineEnabledToken.h"
#include "SardinePaymentType.h"
#include "SardineQuote.h"
#include "Util/JsonBuilder.h"
#include "SardineQuoteType.h"
#include "SardineRegion.h"
#include "SardineSupportedToken.h"
#include "UObject/Object.h"
#include "SardineSupportedRegionsResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineSupportedRegionsResponse
{
	GENERATED_USTRUCT_BODY()

public:
	TArray<FSardineRegion> Regions;

	bool customConstructor = false;

	void construct(FJsonObject& json_in) {}
	void Setup(FJsonObject& json_in)
	{
		for(TSharedPtr<FJsonValue> json : json_in.GetArrayField(TEXT("regions")))
		{
			FSardineRegion Token;
			Token.Setup(*json->AsObject());
			Regions.Add(Token);
		}
	}
};