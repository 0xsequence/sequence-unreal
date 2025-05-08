
#pragma once

#include "CoreMinimal.h"
#include "SeqPage.h"
#include "SeqGetTokenBalancesArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetTokenBalancesArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString accountAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString contractAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        bool includeMetaData = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FSeqPage page;

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