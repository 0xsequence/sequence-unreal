#pragma once

#include "CoreMinimal.h"
#include "PingReturn.generated.h"

USTRUCT(BlueprintType)
struct FPingReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        bool status;
};