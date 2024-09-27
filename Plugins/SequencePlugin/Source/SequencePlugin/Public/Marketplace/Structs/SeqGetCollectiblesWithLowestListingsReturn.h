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
        // Handle the collectible orders array from the JSON input
        const TArray<TSharedPtr<FJsonValue>>* lst;
        if (json_in.TryGetArrayField(TEXT("collectibles"), lst))
        {
            CollectibleOrders.SetNum(lst->Num());

            for (int32 i = 0; i < lst->Num(); i++)
            {
                TSharedPtr<FJsonValue> item = (*lst)[i];
                UE_LOG(LogTemp, Log, TEXT("Processing item %d"), i);

                if (item.IsValid())
                {
                    TSharedPtr<FJsonObject> itemObj = item->AsObject();
                    UE_LOG(LogTemp, Log, TEXT("Successfully converted item %d to object."), i);
                    CollectibleOrders[i].Setup(*itemObj);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to convert item  to object."));
                }
            }

            UE_LOG(LogTemp, Log, TEXT("Found %d collectible orders."), lst->Num());
        }
        else
        {
            // Log if the collectible orders array was not found
            UE_LOG(LogTemp, Warning, TEXT("No  collectible  field found in the JSON input."));
        }
    }
};