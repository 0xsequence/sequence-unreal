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
    UPROPERTY()
        bool healthOK = false;
    UPROPERTY()
        bool indexerEnabled = false;
    UPROPERTY()
        FString startTime = "";
    UPROPERTY()
        double uptime = 0.0;
    UPROPERTY()
        FString ver = "";
    UPROPERTY()
        FString branch = "";
    UPROPERTY()
        FString commitHash = "";
    UPROPERTY()
        int64 chainID = -1;
    UPROPERTY()
        FRuntimeChecks checks;
};