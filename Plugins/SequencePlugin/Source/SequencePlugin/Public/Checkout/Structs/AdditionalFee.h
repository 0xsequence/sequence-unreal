#pragma once

#include "AdditionalFee.Generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FAdditionalFee
{
	GENERATED_USTRUCT_BODY()
public:
	FString Amount;
	FString Receiver;

	FAdditionalFee()
	{
	}
	
	FAdditionalFee(FString Amount, FString Receiver)
	{
		this->Amount = Amount;
		this->Receiver = Receiver;
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
