#pragma once

#include "CoreMinimal.h"
#include "Indexer/Structs/Struct_Data.h"
#include "SeqOrder.h"
#include "Dom/JsonObject.h"
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

        const TSharedPtr<FJsonObject>* metadataJsonIn;
        if (json_in.TryGetObjectField(TEXT("metadata"), metadataJsonIn))
        {
            TokenMetadata.Setup(*metadataJsonIn->Get());
        }

        const TSharedPtr<FJsonObject>* orderJsonIn;
        if (json_in.TryGetObjectField(TEXT("order"), orderJsonIn))
        {
            Order.Setup(*orderJsonIn->Get());
        }
        UE_LOG(LogTemp, Warning, TEXT("seq collectible setup done"));
    }
};