#pragma once 

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "SeqCollectibleOrder.h"
#include "SeqGetOrderReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetOrderReturn
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    FSeqCollectibleOrder Collectible;

    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating
    void Setup(FJsonObject json_in) {};//dummy method to allow for templating in build response!
};