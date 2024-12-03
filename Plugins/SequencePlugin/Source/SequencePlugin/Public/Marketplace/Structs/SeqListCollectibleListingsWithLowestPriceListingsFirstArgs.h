#pragma once 

#include "CoreMinimal.h"
#include "SeqCollectiblesFilter.h" 
#include "SeqMarketplacePage.h"
#include "SeqListCollectibleListingsWithLowestPriceListingsFirstArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqListCollectiblesArgs
{
    GENERATED_USTRUCT_BODY()

public:
    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    FString ContractAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    FSeqCollectiblesFilter Filter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    FSeqMarketplacePage Page;

    bool customGetter = true;

    FString GetArgs()
    {
        FString ret = "{";

        ret += "\"contractAddress\":\"" + ContractAddress + "\"";

        if (Filter.ContainsData())
        {// Check if the Filter has data and append its args if it does
            ret.Append(",\"filter\":");
            ret.Append(Filter.GetArgs());
        }

        // Check if the Page has data and append its args if it does
        if (Page.ContainsData())
        {
            ret.Append(",\"page\":");
            ret.Append(Page.GetArgs());
        }

        ret.Append("}"); // Close the JSON object
        return ret;

    }

};