#pragma once
#include "Marketplace/Marketplace_Enums.h"

#include "GenerateCancelTransactionRequest.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGenerateCancelTransactionRequest
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString CollectionAddress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString Maker;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	TEnumAsByte<EMarketplaceKind> Marketplace;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString OrderId;

	FGenerateCancelTransactionRequest()
	{
		this->CollectionAddress = "";
		this->Maker = "";
		this->Marketplace = UNKNOWN_MK;
		this->OrderId = "";
	}

	FGenerateCancelTransactionRequest(FString CollectionAddress, FString Maker, EMarketplaceKind Marketplace, FString OrderId)
	{
		this->CollectionAddress = CollectionAddress;
		this->Maker = Maker;
		this->Marketplace = Marketplace;
		this->OrderId = OrderId;
	}
	
	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"collectionAddress\":\"" + CollectionAddress + "\",";
		ReturnArgs += "\"maker\":\"" + Maker + "\",";
		ReturnArgs += "\"marketplace\":\"" + UMarketplaceEnumsExtensions::AsString(Marketplace) + "\",";
		ReturnArgs += "\"orderId\":\"" + OrderId + "\"";

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}
};
