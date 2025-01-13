#pragma once 

#include "CoreMinimal.h"
#include "SeqCollectibleOrder.h"
#include "SeqMarketplacePage.h"
#include "SeqGetCollectiblesWithLowestListingsReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetCollectiblesWithLowestListingsReturn
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
        const TArray<TSharedPtr<FJsonValue>>* lst;
        if (json_in.TryGetArrayField(TEXT("collectibles"), lst))
        {
            CollectibleOrders.SetNum(lst->Num());

            for (int32 i = 0; i < lst->Num(); i++)
            {
                TSharedPtr<FJsonValue> item = (*lst)[i];

                if (item.IsValid())
                {
                    TSharedPtr<FJsonObject> itemObj = item->AsObject();
                    CollectibleOrders[i].Setup(*itemObj);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("No  collectibles  field found in the GetCollectiblesWithLowestListings response."));
        }
    }
};