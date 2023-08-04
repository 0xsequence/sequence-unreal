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
        bool includeMetaData = false;

    bool customGetter = true;
    /// <summary>
    /// rename this to GetArgs()! to differentiate it from the Get()'s in the Return UStructs!
    /// </summary>
    /// <returns>The jsonString from this UStruct formatted proper!</returns>
    FString Get()
    {
        FString ret = "{";
        ret += "\"filter\":" + filter.GetArgs();//get the args! MUST Have this!
        if (page.containsData())
            ret += ",\"page\":" + page.GetArgs();
        ret += ",\"includeMetaData\":" + includeMetaData;
        ret += "}";
        return ret;
    }

    FGetTransactionHistoryArgs() {};

    FGetTransactionHistoryArgs(FString addr_in)
    {
        FTransactionHistoryFilter filter_data;
        filter_data.accountAddress = addr_in;
        this->filter = filter_data;
    };
};