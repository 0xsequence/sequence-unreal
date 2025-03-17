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

	void Setup(FJsonObject& json_in)
	{
		json_in.TryGetStringField(TEXT("name"), Name);
		json_in.TryGetBoolField(TEXT("isAllowedOnRamp"), IsAllowedOnRamp);
		json_in.TryGetBoolField(TEXT("isAllowedOnNFT"), IsAllowedOnNFT);
		json_in.TryGetStringArrayField(TEXT("subTypes"), SubTypes);
		json_in.TryGetStringField(TEXT("type"), Type);
		json_in.TryGetStringField(TEXT("subType"), SubType);
	}
};