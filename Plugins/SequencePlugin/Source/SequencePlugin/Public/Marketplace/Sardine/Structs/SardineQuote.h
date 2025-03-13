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

	void Setup(FJsonObject& json_in)
	{
		Quantity = json_in.GetNumberField(TEXT("quantity"));
		Network = json_in.GetStringField(TEXT("network"));
		AssetType = json_in.GetStringField(TEXT("assetType"));
		Total = json_in.GetNumberField(TEXT("total"));
		Currency = json_in.GetStringField(TEXT("currency"));
		ExpiresAt = json_in.GetStringField(TEXT("expiresAt"));
		PaymentType = json_in.GetStringField(TEXT("paymentType"));
		Price = json_in.GetNumberField(TEXT("price"));
		SubTotal = json_in.GetNumberField(TEXT("subTotal"));
		TransactionFee = json_in.GetNumberField(TEXT("transactionFee"));
		NetworkFee = json_in.GetNumberField(TEXT("networkFee"));
		HighNetworkFee = json_in.GetBoolField(TEXT("highNetworkFee"));
		MinTransactionValue = json_in.GetNumberField(TEXT("minTransactionValue"));
		MaxTransactionValue = json_in.GetNumberField(TEXT("maxTransactionValue"));
		LiquidityProvider = json_in.GetStringField(TEXT("liquidityProvider"));
	}
};