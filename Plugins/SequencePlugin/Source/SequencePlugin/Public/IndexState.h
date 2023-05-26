#pragma once

#include "CoreMinimal.h"
#include "IndexState.generated.h"

USTRUCT(BlueprintType)
struct FIndexState
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int64 chainId;
    UPROPERTY()
        int32 lastBlockNum;
};