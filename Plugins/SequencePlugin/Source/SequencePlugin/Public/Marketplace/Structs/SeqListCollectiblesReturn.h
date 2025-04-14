#pragma once 

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "SeqCollectibleOrder.h"
#include "SeqMarketplacePage.h"
#include "SeqListCollectiblesReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqListCollectiblesReturn
{
    GENERATED_USTRUCT_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<FSeqCollectibleOrder> CollectibleOrders;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    FSeqMarketplacePage Page;

    bool customConstructor = false; // Flag to determine the constructor used

    // Dummy constructor method to maintain consistency
    void construct(FJsonObject json_in) {}

    /*
    * Returns a JSON Object for arguments / testing purposes
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);

        // Convert Page object to JSON
        ret->SetObjectField("page", Page.GetJson());

        // Convert CollectibleOrders array to JSON
        TArray<TSharedPtr<FJsonObject>> collectibleOrdersList;
        for (FSeqCollectibleOrder order : CollectibleOrders)
        {
            collectibleOrdersList.Add(order.GetJson());
        }

        // Set the collectible orders as a simple string array in JSON
        ret->SetStringField("collectibleOrders", USequenceSupport::JsonObjListToSimpleString(collectibleOrdersList));
        return ret;
    }

    /*
    * Handles edge cases with Unreal's JSON parsing
    //*/
    void Setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>>* List;
        if (json_in.TryGetArrayField(TEXT("collectibles"), List))
        {
            CollectibleOrders.SetNum(List->Num());

            for (int32 i = 0; i < List->Num(); i++)
            {
                TSharedPtr<FJsonValue> item = (*List)[i];

                if (item.IsValid())
                {
                    TSharedPtr<FJsonObject> itemObj = item->AsObject();
                    CollectibleOrders[i].Setup(*itemObj);
                }
            }
        }
        else
        {
            SEQ_LOG(Warning, TEXT("No  collectibles  field found in the GetCollectiblesWithLowestListings response."));
        }

        Page.setup(*json_in.GetObjectField(TEXT("page")).Get());
    }
};