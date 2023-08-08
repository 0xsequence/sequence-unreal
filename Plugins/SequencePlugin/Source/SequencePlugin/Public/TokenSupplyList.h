#pragma once

#include "CoreMinimal.h"
#include "TokenSupply.h"
#include "TokenSupplyList.generated.h"


/*
    These structs exist because UE T structures don't support nesting BUT do allow us to wrap T structures
    in UStructs allowing us to support nesting
*/

//For use in TokenSuppliesMapReturn
USTRUCT(BlueprintType)
struct FTokenSupplyList
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        TArray<FTokenSupply> token_supply_list;

    //this UStruct only exists because of UE's lack of nested data structures
    //so we need to account for the nesting here!
    FString GetJsonString()
    {
        TArray<TSharedPtr<FJsonObject>> jsonList;
        for (FTokenSupply tItem : token_supply_list)
        {
            jsonList.Add(tItem.GetJson());
        }
        FString ret = UIndexerSupport::jsonObjListToSimpleString(jsonList);

        return ret;
    }

    void setup(TArray<TSharedPtr<FJsonValue>> json_in)
    {
        for (int32 i = 0; i < token_supply_list.Num(); i++)
        {
            TSharedPtr<FJsonObject>* itemObj;
            if (json_in[i].Get()->TryGetObject(itemObj))
            {
                token_supply_list[i].setup(*itemObj->Get());
            }//if
        }//for
    }//setup
};