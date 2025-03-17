#pragma once 

#include "CoreMinimal.h"
#include "SeqCollectiblesFilter.h" 
#include "SeqMarketplacePage.h"
#include "Util/JsonBuilder.h"
#include "SeqGetCollectibleArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetCollectibleArgs
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    FString ContractAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    FString TokenId;

    bool customGetter = true;

    FString GetArgs()
    {
        FJsonBuilder Builder;

        Builder.AddString("contractAddress", ContractAddress);
        Builder.AddString("tokenId", TokenId);
        
        return Builder.ToString();
    }

};