#pragma once

#include "CoreMinimal.h"
#include "GetChainIDArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetChainIDArgs
{
    GENERATED_USTRUCT_BODY()
public:
    //this guy is a nothing burger?

    bool customGetter = false;
    FString Get() { return ""; }
};