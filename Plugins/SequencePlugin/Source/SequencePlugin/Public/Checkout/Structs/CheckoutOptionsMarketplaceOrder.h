#pragma once
#include "Marketplace/Marketplace_Enums.h"

#include "CheckoutOptionsMarketplaceOrder.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FCheckoutOptionsMarketplaceOrder
{
	GENERATED_USTRUCT_BODY()

public:
	FString ContractAddress;
	FString OrderId;
	EMarketplaceKind Marketplace;

	FCheckoutOptionsMarketplaceOrder()
	{
		this->ContractAddress = "";
		this->OrderId = "";
		this->Marketplace = EMarketplaceKind::UNKNOWN_MK;
	}

	FCheckoutOptionsMarketplaceOrder(FString ContractAddress, FString OrderId, const EMarketplaceKind Marketplace)
	{
		this->ContractAddress = ContractAddress;
		this->OrderId = OrderId;
		this->Marketplace = Marketplace;
	}

	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"contractAddress\":\"" + ContractAddress + "\",";
		ReturnArgs += "\"orderId\":\"" + OrderId + "\",";
		ReturnArgs += "\"marketplace\":\"" + UEnum::GetValueAsString(Marketplace) + "\"";

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}
};
