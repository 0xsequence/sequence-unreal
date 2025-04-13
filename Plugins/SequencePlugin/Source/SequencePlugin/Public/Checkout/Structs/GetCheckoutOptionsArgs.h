#pragma once
#include "CheckoutOptionsMarketplaceOrder.h"

#include "GetCheckoutOptionsArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetCheckoutOptionsArgs
{
	GENERATED_USTRUCT_BODY()

private:
	FString GetOrdersDataArgs() const
	{
		FString OrdersDataArgs = "";

		for (uint16 i = 0; i < Orders.Num(); i++)
		{
			if (i < Orders.Num() - 1)
			{
				OrdersDataArgs += Orders[i].GetArgs() + ",";
			}
			else
			{
				OrdersDataArgs += Orders[i].GetArgs();
			}
		}
		
		return OrdersDataArgs;
	}
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString WalletAddress;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FCheckoutOptionsMarketplaceOrder> Orders;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int64 AdditionalFee;

	FGetCheckoutOptionsArgs()
	{
		this->WalletAddress = "";
		this->Orders = TArray<FCheckoutOptionsMarketplaceOrder>();
		this->AdditionalFee = 0;
	}

	FGetCheckoutOptionsArgs(FString WalletAddress, const TArray<FCheckoutOptionsMarketplaceOrder>& Orders, const int64 AdditionalFee)
	{
		this->WalletAddress = WalletAddress;
		this->Orders = Orders;
		this->AdditionalFee = AdditionalFee;
	}
	
	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"wallet\":\"" + WalletAddress + "\",";
		ReturnArgs += "\"orders\":[" + GetOrdersDataArgs() + "],";
		ReturnArgs += "\"additionalFee\":" + FString::Printf(TEXT("%lld"), AdditionalFee) + "";

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}
};
