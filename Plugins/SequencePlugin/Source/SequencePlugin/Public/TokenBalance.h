#pragma once

#include "CoreMinimal.h"
#include "TokenSupply.h"
#include "TokenBalance.generated.h"

USTRUCT(BlueprintType)
struct FTokenBalance
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        TMap<FString, TArray<FTokenSupply>> supplies;
};