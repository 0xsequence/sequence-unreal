// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "RuntimeChecks.generated.h"

USTRUCT(BlueprintType)
struct FRuntimeChecks
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
        bool running = false;
    UPROPERTY(BlueprintReadWrite)
        FString syncMode = "";
    UPROPERTY(BlueprintReadWrite)
        int64 lastBlockNum = -1;
};