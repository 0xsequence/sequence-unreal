#pragma once

#include "CoreMinimal.h"
#include "RuntimeChecks.generated.h"

USTRUCT(BlueprintType)
struct FRuntimeChecks
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        bool running;
    UPROPERTY()
        FString syncMode;
    UPROPERTY()
        int64 lastBlockNum;
};