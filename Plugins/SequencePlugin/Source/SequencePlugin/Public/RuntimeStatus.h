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
        bool healthOK;
    UPROPERTY()
        bool indexerEnabled;
    UPROPERTY()
        FString startTime;
    UPROPERTY()
        double uptime;
    UPROPERTY()
        FString ver;
    UPROPERTY()
        FString branch;
    UPROPERTY()
        FString commitHash;
    UPROPERTY()
        int64 chainID;
    UPROPERTY()
        FRuntimeChecks checks;
};