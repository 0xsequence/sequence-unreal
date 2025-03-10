#pragma once

#include "CoreMinimal.h"
#include "SardineRegionPaymentMethod.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineRegionPaymentMethod
{
	GENERATED_USTRUCT_BODY()

public:
	FString Name;
	bool IsAllowedOnRamp;
	bool IsAllowedOnNFT;
	TArray<FString> SubTypes;
	FString Type;
	FString SubType;
};