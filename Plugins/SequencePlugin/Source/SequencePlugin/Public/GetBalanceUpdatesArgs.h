#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "GetBalanceUpdatesArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetBalanceUpdatesArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        int32 lastUpdateID;
    UPROPERTY()
        FPage page;
};