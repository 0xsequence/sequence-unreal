#pragma once

#include "CoreMinimal.h"
#include "RuntimeStatus.h"
#include "RuntimeStatusReturn.generated.h"

USTRUCT(BlueprintType)
struct FRuntimeStatusReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FRuntimeStatus status;
};