#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "GetTokenBalancesArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenBalancesArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString accountAddress;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        bool includeMetaData;
    UPROPERTY()
        FPage page;
};
