#pragma once

#include "CoreMinimal.h"
#include "CheckoutOptionsItem.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FCheckoutOptionsItem
{
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString TokenId;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString Quantity;
	
	FCheckoutOptionsItem()
	{
		this->TokenId = "";
		this->Quantity = "";
	}

	FCheckoutOptionsItem(FString TokenId, FString Quantity)
	{
		this->TokenId = TokenId;
		this->Quantity = Quantity;
	}
};
