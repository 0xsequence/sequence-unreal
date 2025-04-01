// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include <string>

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SardineNFTCheckout.h"
#include "SardineGetNFTCheckoutTokenResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineGetNFTCheckoutTokenResponse
{
	GENERATED_USTRUCT_BODY()

public:
	FSardineNFTCheckout Checkout;

	bool customConstructor = false;

	void construct(FJsonObject& json_in) {}
	void Setup(FJsonObject& json_in)
	{
		const auto CheckoutObject = json_in.GetObjectField(TEXT("resp"));
		Checkout.Setup(*CheckoutObject);
	}
};