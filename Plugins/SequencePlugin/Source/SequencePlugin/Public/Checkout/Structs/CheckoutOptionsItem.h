// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "CheckoutOptionsItem.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FCheckoutOptionsItem
{
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString TokenId;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
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
