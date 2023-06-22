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
    //UPROPERTY()
      // FPage page;
    UPROPERTY()
        bool includeMetaData = false;

    bool customGetter = false;
    FString Get() {return"";};

    FGetTransactionHistoryArgs() {};

    FGetTransactionHistoryArgs(FString addr_in)
    {
        FTransactionHistoryFilter filter_data;
        filter_data.accountAddress = addr_in;
        this->filter = filter;
    };
};