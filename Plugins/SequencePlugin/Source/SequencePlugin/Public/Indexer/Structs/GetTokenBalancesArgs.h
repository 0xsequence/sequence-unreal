// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "GetTokenBalancesArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenBalancesArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
        FString accountAddress = "";
    UPROPERTY(BlueprintReadWrite)
        FString contractAddress = "";
    UPROPERTY(BlueprintReadWrite)
        bool includeMetaData = false;
    UPROPERTY(BlueprintReadWrite)
        FPage page;

    bool customGetter = true;

    /*
    * Used to get the JsonObjectString formed by this struct
    * used for args and testing
    */
    FString GetArgs()
    {
        FString ret = "{";

        ret += "\"accountAddress\":\"" + accountAddress + "\"";

        if (contractAddress.Len() > 0)
        {
            ret += ",\"contractAddress\":\"" + contractAddress + "\"";
        }

        ret.Append(",\"includeMetaData\":");
        ret.Append(includeMetaData ? "true" : "false");

        if (page.containsData())
        {
            ret.Append(",\"page\":");
            ret.Append(page.GetArgs());
        }

        ret += "}";
        return ret; 
    }
};