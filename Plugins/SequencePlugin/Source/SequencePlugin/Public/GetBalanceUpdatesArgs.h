#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "GetBalanceUpdatesArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetBalanceUpdatesArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString contractAddress = "";
    UPROPERTY()
        int32 lastUpdateID = -1;
    UPROPERTY()
        FPage page;
    bool customGetter = true;
    FString GetArgs()
    {
        FString ret = "{";

        ret += "\"contractAddress\":\"" + contractAddress + "\"";

        if (lastUpdateID != -1)
        {
            ret += ",\"lastUpdateID\":";
            ret.AppendInt(lastUpdateID);
        }

        if (page.containsData())
        {
            ret += ",\"page\":" + page.GetArgs();
        }

        ret += "}";
        return ret; 
    };
};