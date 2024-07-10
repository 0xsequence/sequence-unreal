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
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool healthOK = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool indexerEnabled = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString startTime = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        double uptime = 0.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString ver = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString branch = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString commitHash = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int64 chainID = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FRuntimeChecks checks;
};