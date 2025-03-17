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

	void Setup(FJsonObject& json_in)
	{
		json_in.TryGetStringField(TEXT("code"), Code);
		json_in.TryGetStringField(TEXT("name"), Name);
		json_in.TryGetBoolField(TEXT("isAllowedOnRamp"), IsAllowedOnRamp);
		json_in.TryGetBoolField(TEXT("isAllowedOnNFT"), IsAllowedOnNFT);
	}
};