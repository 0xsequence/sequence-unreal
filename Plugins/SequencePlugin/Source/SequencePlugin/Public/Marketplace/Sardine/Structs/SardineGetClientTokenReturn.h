#pragma once

#include "SardineGetClientTokenReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FsardineGetClientTokenReturn
{
	GENERATED_USTRUCT_BODY()

	FString Token;

	bool customConstructor = false; // Flag to determine the constructor used

	// Dummy constructor method to maintain consistency
	void construct(FJsonObject json_in) {}

	/*
	* Handles edge cases with Unreal's JSON parsing
	*/
	void Setup(FJsonObject json_in)
	{
		Token = json_in.GetStringField(TEXT("token"));
	}
};
