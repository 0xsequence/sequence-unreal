#pragma once

#include "CoreMinimal.h"
#include "SardineToken.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineToken
{
	GENERATED_USTRUCT_BODY()

public:
	FString Network;
	FString AssetSymbol;
	FString AssetName;
	FString ChainID;
	FString TokenName;
	FString Token;
	FString TokenAddress;
};