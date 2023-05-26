#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "TransactionHistoryFilter.h"
#include "GetTransactionHistoryArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetTransactionHistoryArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FTransactionHistoryFilter filter;
    UPROPERTY()
        FPage page;
    UPROPERTY()
        bool includeMetaData;
};