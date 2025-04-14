#pragma once
#include "Checkout/Enums/CheckoutOptions.h"
#include "Dom/JsonObject.h"
#include "GetCheckoutOptionsResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetCheckoutOptionsResponse
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "0xSequence - Checkout")
	FCheckoutOptions Options;

	FGetCheckoutOptionsResponse()
	{
		this->Options = FCheckoutOptions();
	}

	FGetCheckoutOptionsResponse(const FCheckoutOptions& Options)
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
			if (const TSharedPtr<FJsonValue> OptionsValue = JSON_In.TryGetField(TEXT("options")); OptionsValue != nullptr)
			{
				TSharedPtr<FJsonObject> OptionsJsonObject = OptionsValue->AsObject();
				this->Options = FCheckoutOptions();
				this->Options.Setup(*OptionsJsonObject);
			}
		}
	}
};
