#pragma once

#include "CoreMinimal.h"
#include "GetChainIDReturn.generated.h"

USTRUCT(BlueprintType)
struct FGetChainIDReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int64 chainID;
};