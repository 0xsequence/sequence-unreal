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

		Builder.AddString(TEXT("ReferenceId"), ReferenceId);
		Builder.AddInt(TEXT("ExpiresIn"), ExpiresIn);
		Builder.AddField(TEXT("PaymentMethodTypeConfig"), PaymentMethodTypeConfig.GetArgs());
		Builder.AddString(TEXT("Name"), Name);
		Builder.AddString(TEXT("ImageUrl"), ImageUrl);
		Builder.AddString(TEXT("Network"), Network);
		Builder.AddString(TEXT("RecipientAddress"), RecipientAddress);
		Builder.AddString(TEXT("ContractAddress"), ContractAddress);
		Builder.AddString(TEXT("Platform"), Platform);
		Builder.AddString(TEXT("ExecutionType"), ExecutionType);
		Builder.AddString(TEXT("BlockchainNftId"), BlockchainNftId);
		Builder.AddInt(TEXT("Quantity"), Quantity);
		Builder.AddInt(TEXT("Decimals"), Decimals);
		Builder.AddString(TEXT("TokenAmount"), TokenAmount);
		Builder.AddString(TEXT("TokenAddress"), TokenAddress);
		Builder.AddString(TEXT("TokenSymbol"), TokenSymbol);
		Builder.AddInt(TEXT("TokenDecimals"), TokenDecimals);
		Builder.AddString(TEXT("CallData"), CallData);
	
		return Builder.ToString();
	}
};