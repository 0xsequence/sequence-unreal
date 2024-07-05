// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "RuntimeChecks.h"
#include "RuntimeStatus.generated.h"

USTRUCT(BlueprintType)
struct FRuntimeStatus
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
        bool healthOK = false;
    UPROPERTY(BlueprintReadWrite)
        bool indexerEnabled = false;
    UPROPERTY(BlueprintReadWrite)
        FString startTime = "";
    UPROPERTY(BlueprintReadWrite)
        double uptime = 0.0;
    UPROPERTY(BlueprintReadWrite)
        FString ver = "";
    UPROPERTY(BlueprintReadWrite)
        FString branch = "";
    UPROPERTY(BlueprintReadWrite)
        FString commitHash = "";
    UPROPERTY(BlueprintReadWrite)
        int64 chainID = -1;
    UPROPERTY(BlueprintReadWrite)
        FRuntimeChecks checks;
};