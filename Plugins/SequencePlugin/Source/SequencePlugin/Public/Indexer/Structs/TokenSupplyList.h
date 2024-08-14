// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TArray<FTokenSupply> token_supply_list;

    /*
    * This ustruct only exists because of UE's lack of nested data structures,
    * so we account for the nesting here, this will return a JsonObjectString accounting
    * for the nesting caused by this struct
    */
    FString GetJsonString()
    {
        TArray<TSharedPtr<FJsonObject>> jsonList;
        for (FTokenSupply tItem : token_supply_list)
        {
            jsonList.Add(tItem.GetJson());
        }
        FString ret = USequenceSupport::JsonObjListToSimpleString(jsonList);

        return ret;
    }

    /*
    * Used to handle edge cases with unreal's json parsing
    */
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