// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "SardinePaymentType.h"
#include "Util/JsonBuilder.h"
#include "SardineQuoteType.h"
#include "UObject/Object.h"
#include "SardineGetQuoteArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineGetQuoteArgs
{
	GENERATED_USTRUCT_BODY()

private:
	inline static const FString DefaultCurrency = "USD";

public:
	FString AssetType;
	FString Network;
	u_long Total;
	FString Currency;
	ESardinePaymentType PaymentType;
	ESardineQuoteType QuoteType;
	FString WalletAddress;

	bool customGetter = true;

	FString GetArgs()
	{
		FJsonBuilder Builder;

		Builder.AddString(TEXT("assetType"), AssetType);
		Builder.AddString(TEXT("network"), Network);
		Builder.AddField(TEXT("total"), FString::SanitizeFloat(Total));
		Builder.AddString(TEXT("currency"), Currency == "" ? DefaultCurrency : Currency);
		Builder.AddString(TEXT("paymentType"), UEnum::GetValueAsString(PaymentType).ToLower());
		Builder.AddString(TEXT("quoteType"), UEnum::GetValueAsString(QuoteType).ToLower());
		Builder.AddString(TEXT("walletAddress"), WalletAddress);
	
		return Builder.ToString();
	}
};