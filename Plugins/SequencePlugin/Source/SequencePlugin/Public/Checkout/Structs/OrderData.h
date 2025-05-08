#pragma once

#include "OrderData.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FOrderData
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString OrderId;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString Quantity;

	FOrderData()
	{
		OrderId = "";
		Quantity = "";
	}

	FOrderData(FString OrderId, FString Quantity)
	{
		this->OrderId = OrderId;
		this->Quantity = Quantity;
	}
	
	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"orderId\":\"" + OrderId + "\",";
		ReturnArgs += "\"quantity\":\"" + Quantity + "\"";

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}
};
