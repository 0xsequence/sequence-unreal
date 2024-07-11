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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        bool healthOK = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        bool indexerEnabled = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString startTime = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        double uptime = 0.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString ver = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString branch = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString commitHash = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        int64 chainID = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FRuntimeChecks checks;
};