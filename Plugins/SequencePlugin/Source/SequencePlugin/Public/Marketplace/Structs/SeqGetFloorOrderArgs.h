#pragma once 

#include "CoreMinimal.h"
#include "SeqCollectiblesFilter.h" 
#include "SeqMarketplacePage.h"
#include "Util/JsonBuilder.h"
#include "SeqGetFloorOrderArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetFloorOrderArgs
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    FString ContractAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    FSeqCollectiblesFilter Filter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    FSeqMarketplacePage Page;

    bool customGetter = true;

    FString GetArgs()
    {
        FJsonBuilder Builder;

        Builder.AddString("contractAddress", ContractAddress);

        if (Filter.ContainsData())
        {// Check if the Filter has data and append its args if it does
            Builder.AddField("filter", Filter.GetArgs());
        }

        // Check if the Page has data and append its args if it does
        if (Page.ContainsData())
        {
            Builder.AddString("page", Page.GetArgs());
        }

        return Builder.ToString();
    }

};