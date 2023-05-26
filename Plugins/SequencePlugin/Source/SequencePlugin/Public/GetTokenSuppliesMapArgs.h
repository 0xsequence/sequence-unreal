#pragma once

#include "CoreMinimal.h"
#include "TokenList.h"
#include "GetTokenSuppliesMapArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenSuppliesMapArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        TMap<FString, FTokenList> tokenMap;
    UPROPERTY()
        bool includeMetaData;
};