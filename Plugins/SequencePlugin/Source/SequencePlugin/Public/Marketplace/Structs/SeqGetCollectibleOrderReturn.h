#pragma once

#include "CoreMinimal.h"
#include "SeqCollectibleOrder.h"
#include "SeqCurrency.h"
#include "SeqGetCollectibleOrderReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetCollectibleOrderReturn
{
    GENERATED_USTRUCT_BODY()

public:
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating
    void Setup(FJsonObject json_in) {};//dummy method to allow for templating in build response!
    
    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FSeqCollectibleOrder Order;
};