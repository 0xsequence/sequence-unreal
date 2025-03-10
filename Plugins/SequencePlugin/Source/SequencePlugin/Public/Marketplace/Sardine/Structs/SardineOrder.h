#pragma once

#include "CoreMinimal.h"
#include "SardineOrder.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineOrder
{
	GENERATED_USTRUCT_BODY()

public:
	FString ID;
	FString CreatedAt;
	FString ReferenceID;
	FString Status;
	FString FiatCurrency;
	double FiatExchangeRateUSD;
	FString TransactionID;
	FString ExpiresAt;
	double Total;
	double SubTotal;
	double TransactionFee;
	double NetworkFee;
	FString PaymentCurrency;
	FString PaymentMethodType;
	FString TransactionType;
	FString Name;
	int Price;
	FString ImageURL;
	FString ContractAddress;
	FString TransactionHash;
	FString RecipientAddress;
};