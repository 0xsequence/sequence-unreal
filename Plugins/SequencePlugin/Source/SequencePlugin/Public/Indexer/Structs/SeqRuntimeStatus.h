// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqRuntimeChecks.h"
#include "SeqRuntimeStatus.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqRuntimeStatus
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        bool healthOK = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        bool indexerEnabled = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString startTime = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        double uptime = 0.0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString ver = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString branch = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString commitHash = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        int64 chainID = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FSeqRuntimeChecks checks;
};