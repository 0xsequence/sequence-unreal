// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FTransactionHistoryFilter filter;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FPage page;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        bool includeMetaData = false;

    bool customGetter = true;
    /// <summary>
    /// 
    /// </summary>
    /// <returns>The jsonString from this UStruct formatted proper!</returns>
    FString GetArgs()
    {
        FString ret = "{";
        ret.Append("\"filter\":");
        ret.Append(filter.GetArgs());//get the args! MUST Have this!
        if (page.containsData())
        {
            ret.Append(",\"page\":");
            ret.Append(page.GetArgs());
        }

        ret.Append(",\"includeMetaData\":");
        ret.Append(includeMetaData ? "true" : "false");

        ret.Append("}");

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