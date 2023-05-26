#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "TokenBalance.h"
#include "GetBalanceUpdatesReturn.generated.h"

USTRUCT(BlueprintType)
struct FGetBalanceUpdatesReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FPage page;
    UPROPERTY()
        TArray<FTokenBalance> balances;
};