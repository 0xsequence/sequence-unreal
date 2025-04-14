#pragma once

#include "Dom/JsonObject.h"
#include "AdditionalFee.Generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FAdditionalFee
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString Amount;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence - Checkout")
	FString Receiver;

	FAdditionalFee()
	{
	}
	
	FAdditionalFee(FString Amount, FString Receiver)
	{
		this->Amount = Amount;
		this->Receiver = Receiver;
	}

	bool IsEmpty() const
	{
		return Amount.IsEmpty() && Receiver.IsEmpty();
	}
	
	bool customGetter = true;
	FString GetArgs() const
	{
		FString ReturnArgs = "{";

		ReturnArgs += "\"amount\":\"" + Amount + "\",";
		ReturnArgs += "\"receiver\":\"" + Receiver + "\"";

		ReturnArgs.Append("}"); // Close the JSON object
		
		return ReturnArgs;
	}
};
