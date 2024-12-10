#pragma once

#include "CoreMinimal.h"
#include "Indexer/Structs/Struct_Data.h"
#include "SeqOrder.h"
#include "Marketplace/Marketplace_Enums.h"
#include "SeqCollectibleOrder.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqCollectibleOrder
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FSeqTokenMetaData TokenMetadata;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Order")
    FSeqOrder Order;

    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

        JsonObject->SetObjectField("metadata", TokenMetadata.GetJson());

        JsonObject->SetObjectField("order", Order.GetJson());

        return JsonObject;
    }

    void Setup(FJsonObject json_in)
    {
        UE_LOG(LogTemp, Warning, TEXT("seq collectible setup start"));
        
        const TSharedPtr<FJsonObject>* item;
        if (json_in.TryGetObjectField(TEXT("metadata"), item))
        {
            TokenMetadata.Setup(*item->Get());
        }

        const TSharedPtr<FJsonObject>* orderItem;
        if (json_in.TryGetObjectField(TEXT("order"), orderItem))
        {
            Order.Setup(*orderItem->Get());
        }

        UE_LOG(LogTemp, Warning, TEXT("seq collectible setup done"));
    }
};