#pragma once

#include "CoreMinimal.h"
#include "SardineRegionPaymentMethod.h"
#include "SardineRegionState.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSardineRegionState
{
	GENERATED_USTRUCT_BODY()

public:
	FString Code;
	FString Name;
	bool IsAllowedOnRamp;
	bool IsAllowedOnNFT;
};