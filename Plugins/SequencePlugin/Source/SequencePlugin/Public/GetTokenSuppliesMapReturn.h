#pragma once

#include "CoreMinimal.h"
#include "TokenSupply.h"
#include "GetTokenSuppliesMapReturn.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenSuppliesMapReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        TMap<FString, TArray<FTokenSupply>> supplies;
};