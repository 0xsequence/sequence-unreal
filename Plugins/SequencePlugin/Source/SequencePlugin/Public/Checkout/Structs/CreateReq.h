#pragma once

#include "CreateReq.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FCreateReq
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString TokenId;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Quantity;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Expiry;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString CurrencyAddress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString PricePerToken;

	FCreateReq()
	{
		this->TokenId = "";
		this->Quantity = "";
		this->Expiry = "";
		this->CurrencyAddress = "";
		this->PricePerToken = "";
	}
	
	FCreateReq(FString TokenId, FString Quantity, FString Expiry, FString CurrencyAddress, FString PricePerToken)
	{
		this->TokenId = TokenId;
		this->Quantity = Quantity;
		this->Expiry = Expiry;
		this->CurrencyAddress = CurrencyAddress;
		this->PricePerToken = PricePerToken;
	}
	
	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"tokenId\":\"" + TokenId + "\",";
		ReturnArgs += "\"quantity\":\"" + Quantity + "\",";
		ReturnArgs += "\"expiry\":\"" + Expiry + "\",";
		ReturnArgs += "\"currencyAddress\":\"" + CurrencyAddress + "\",";
		ReturnArgs += "\"pricePerToken\":\"" + PricePerToken + "\"";

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}

	void Setup(FJsonObject& JSON_In)
	{
		TokenId = JSON_In.GetStringField(TEXT("tokenId"));
		Quantity = JSON_In.GetStringField(TEXT("quantity"));
		Expiry = JSON_In.GetStringField(TEXT("expiry"));
		CurrencyAddress = JSON_In.GetStringField(TEXT("currencyAddress"));
		PricePerToken = JSON_In.GetStringField(TEXT("pricePerToken"));
	}
};
