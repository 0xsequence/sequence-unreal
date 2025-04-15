#pragma once
#include "Checkout/Enums/CheckoutOptions.h"

#include "GetCheckoutOptionsResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetCheckoutOptionsResponse
{
	GENERATED_USTRUCT_BODY()

	public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECheckoutOptions Options;

	FGetCheckoutOptionsResponse()
	{
		this->Options = ECheckoutOptions::CryptoPurchase;
	}

	FGetCheckoutOptionsResponse(const ECheckoutOptions Options)
	{
		this->Options = Options;
	}
	
	bool customConstructor = false;

	void Construct(FJsonObject JSON_In)
	{
	}

	void Setup(const FJsonObject& JSON_In)
	{
		if (JSON_In.HasField(TEXT("options")))
		{
			this->Options = static_cast<ECheckoutOptions>(JSON_In.GetIntegerField(TEXT("options")));
		}
	}
};
