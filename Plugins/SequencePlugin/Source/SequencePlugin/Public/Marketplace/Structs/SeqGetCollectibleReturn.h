#pragma once

#include "CoreMinimal.h"
#include "SeqCollectibleOrder.h"
#include "SeqCurrency.h"
#include "SeqGetCollectibleReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetCollectibleReturn
{
    GENERATED_USTRUCT_BODY()

public:
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating
    void Setup(FJsonObject json_in)
    {
        const TSharedPtr<FJsonObject>* obj;
        if(json_in.TryGetObjectField(TEXT("metadata"), obj))
        {
            MetaData.Setup(*obj->Get());
        }
    };
    
    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FSeqTokenMetaData MetaData;
};