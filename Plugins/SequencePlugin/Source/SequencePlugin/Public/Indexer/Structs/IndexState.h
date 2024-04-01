// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "IndexState.generated.h"

USTRUCT(BlueprintType)
struct FIndexState
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int64 chainId = -1;
    UPROPERTY()
        int32 lastBlockNum = -1;
};