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

        // Add TokenMetadata to the JSON
        JsonObject->SetObjectField("metadata", TokenMetadata.GetJson());

        // Add Order to the JSON
        JsonObject->SetObjectField("order", Order.GetJson());

        return JsonObject;
    }

    void Setup(FJsonObject json_in)
    {
        // Log the entire input JSON (optional, only for deep debugging)
        FString inputJsonString;
        const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&inputJsonString);
        FJsonSerializer::Serialize(MakeShared<FJsonObject>(json_in), Writer);
        UE_LOG(LogTemp, Log, TEXT("Received JSON input: %s"), *inputJsonString);

        const TSharedPtr<FJsonObject>* item;
        if (json_in.TryGetObjectField(TEXT("metadata"), item))
        {
            // Log that tokenMetaData was found
            UE_LOG(LogTemp, Log, TEXT("tokenMetaData field found in the JSON."));

            // Log the content of tokenMetaData before passing to Setup
            FString tokenMetaDataJsonString;
            const TSharedRef<TJsonWriter<>> TokenMetaDataWriter = TJsonWriterFactory<>::Create(&tokenMetaDataJsonString);
            FJsonSerializer::Serialize(item->ToSharedRef(), TokenMetaDataWriter);
            UE_LOG(LogTemp, Log, TEXT("tokenMetaData content: %s"), *tokenMetaDataJsonString);

            TokenMetadata.Setup(*item->Get());

            // Log successful tokenMetaData setup
            UE_LOG(LogTemp, Log, TEXT("TokenMetadata setup completed."));
        }
        else
        {
            // Log if tokenMetaData was not found
            UE_LOG(LogTemp, Warning, TEXT("tokenMetaData field not found in the JSON."));
        }

        // Check and set Order
        const TSharedPtr<FJsonObject>* orderItem;
        if (json_in.TryGetObjectField(TEXT("order"), orderItem))
        {
            // Log that order field was found
            UE_LOG(LogTemp, Log, TEXT("order field found in the JSON."));

            // Log the content of order before passing to Setup
            FString orderJsonString;
            const TSharedRef<TJsonWriter<>> OrderWriter = TJsonWriterFactory<>::Create(&orderJsonString);
            FJsonSerializer::Serialize(orderItem->ToSharedRef(), OrderWriter);
            UE_LOG(LogTemp, Log, TEXT("Order content: %s"), *orderJsonString);

            Order.Setup(*orderItem->Get());

            // Log successful Order setup
            UE_LOG(LogTemp, Log, TEXT("Order setup completed."));
        }
        else
        {
            // Log if order was not found
            UE_LOG(LogTemp, Warning, TEXT("order field not found in the JSON."));
        }

        // Log the end of the Setup function
        UE_LOG(LogTemp, Log, TEXT("FSeqCollectibleOrder Setup completed."));
    }
};