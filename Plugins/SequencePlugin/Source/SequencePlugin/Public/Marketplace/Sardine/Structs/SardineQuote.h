#pragma once

#include "CoreMinimal.h"
#include "SardineQuote.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineQuote
{
	GENERATED_USTRUCT_BODY()

public:
	double Quantity;
	FString Network;
	FString AssetType;
	double Total;
	FString Currency;
	FString ExpiresAt;
	FString PaymentType;
	double Price;
	double SubTotal;
	double TransactionFee;
	double NetworkFee;
	bool HighNetworkFee;
	double MinTransactionValue;
	double MaxTransactionValue;
	FString LiquidityProvider;
};