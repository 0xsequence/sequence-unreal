#pragma once 

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "SeqCollectibleOrder.h"
#include "SeqMarketplacePage.h"
#include "SeqListCollectibleListingsReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqListCollectibleListingsReturn
{
    GENERATED_USTRUCT_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectibles")
    TArray<FSeqCollectibleOrder> Listings;

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
        for (FSeqCollectibleOrder order : Listings)
        {
            collectibleOrdersList.Add(order.GetJson());
        }

        // Set the collectible orders as a simple string array in JSON
        ret->SetStringField("listings", USequenceSupport::JsonObjListToSimpleString(collectibleOrdersList));
        return ret;
    }

    /*
    * Handles edge cases with Unreal's JSON parsing
    //*/
    void Setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>>* lst;
        if (json_in.TryGetArrayField(TEXT("listings"), lst))
        {
            Listings.SetNum(lst->Num());

            for (int32 i = 0; i < lst->Num(); i++)
            {
                TSharedPtr<FJsonValue> item = (*lst)[i];

                if (item.IsValid())
                {
                    TSharedPtr<FJsonObject> itemObj = item->AsObject();
                    Listings[i].Setup(*itemObj);
                }
            }
        }
        else
        {
            SEQ_LOG(Warning, TEXT("No  collectibles  field found in the GetCollectiblesWithLowestListings response."));
        }
    }
};