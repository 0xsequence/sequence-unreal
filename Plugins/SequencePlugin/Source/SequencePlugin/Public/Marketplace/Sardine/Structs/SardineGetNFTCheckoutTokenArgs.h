// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "SardinePaymentMethodTypeConfig.h"
#include "SardinePaymentType.h"
#include "Util/JsonBuilder.h"
#include "SardineQuoteType.h"
#include "UObject/Object.h"
#include "SardineGetNFTCheckoutTokenArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineGetNFTCheckoutTokenArgs
{
	GENERATED_USTRUCT_BODY()

public:
	FString ReferenceId;
	uint ExpiresIn;
	FSardinePaymentMethodTypeConfig PaymentMethodTypeConfig;
	FString Name;
	FString ImageUrl;
	FString Network;
	FString RecipientAddress;
	FString ContractAddress;
	FString Platform;
	FString ExecutionType;
	FString BlockchainNftId;
	long Quantity;
	long Decimals;
	FString TokenAmount;
	FString TokenAddress;
	FString TokenSymbol;
	long TokenDecimals;
	FString CallData;
	
	bool customGetter = true;

	FString GetArgs()
	{
		FJsonBuilder Builder;

		Builder.AddString(TEXT("name"), Name);
		Builder.AddString(TEXT("imageUrl"), ImageUrl);
		Builder.AddString(TEXT("network"), Network);
		Builder.AddString(TEXT("recipientAddress"), RecipientAddress);
		Builder.AddString(TEXT("blockchainNftId"), BlockchainNftId);
		Builder.AddString(TEXT("contractAddress"), ContractAddress);
		Builder.AddInt(TEXT("decimals"), Decimals);
		Builder.AddInt(TEXT("quantity"), Quantity);
		Builder.AddString(TEXT("tokenAmount"), TokenAmount);
		Builder.AddString(TEXT("tokenAddress"), TokenAddress);
		Builder.AddString(TEXT("tokenSymbol"), TokenSymbol);
		Builder.AddInt(TEXT("tokenDecimals"), TokenDecimals);
		Builder.AddString(TEXT("callData"), CallData);
		Builder.AddString(TEXT("platform"), Platform);
		
		// Builder.AddInt(TEXT("expiresIn"), ExpiresIn);
		// Builder.AddString(TEXT("referenceId"), ReferenceId);

		FString Params = "{\"params\":" + Builder.ToString() + "}";
		return Params;
	}
};