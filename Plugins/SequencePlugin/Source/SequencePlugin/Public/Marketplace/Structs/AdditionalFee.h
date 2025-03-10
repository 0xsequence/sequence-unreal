#pragma once

#include "CoreMinimal.h"
#include "AdditionalFee.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FAdditionalFee
{
	GENERATED_USTRUCT_BODY()

public:
	FString Amount;
	FString Receiver;
};