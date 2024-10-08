// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqPage.h"
#include "SeqGetBalanceUpdatesArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetBalanceUpdatesArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString contractAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        int32 lastUpdateID = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FSeqPage page;
    bool customGetter = true;
    /*
    * Gets the jsonObjectString formed by this struct
    */
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