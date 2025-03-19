#pragma once

#include "SardineQuote.h"
#include "SardineGetQuoteReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineGetQuoteReturn
{
	GENERATED_USTRUCT_BODY()

	FSardineQuote Quote;

	bool customConstructor = false; // Flag to determine the constructor used

	// Dummy constructor method to maintain consistency
	void construct(FJsonObject json_in) {}

	/*
	* Handles edge cases with Unreal's JSON parsing
	*/
	void Setup(FJsonObject json_in)
	{
		Quote.Setup(*json_in.GetObjectField(TEXT("quote")));
	}
};
