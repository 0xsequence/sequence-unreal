#pragma once

#include "CoreMinimal.h"
#include "SardineNFTCheckout.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineNFTCheckout
{
	GENERATED_USTRUCT_BODY()

public:
	FString Token;
	FString ExpiresAt;
	FString OrderID;
};