// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "Indexer/Indexer_Enums.h"
#include "TokenSupply.h"
#include "GetTokenSuppliesReturn.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenSuppliesReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FPage page;
    UPROPERTY()
        TEnumAsByte<EContractType> contractType = EContractType::ERC1155;
    UPROPERTY()
        TArray<FTokenSupply> tokenIDs;
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating

    /*
    * Used to return a json object formed by this struct used for
    * args & testing purposes
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);

        ret.Get()->SetObjectField("page", page.GetJson());
        ret.Get()->SetStringField("contractType", UEnum::GetValueAsString(contractType.GetValue()));
        TArray<TSharedPtr<FJsonObject>> jList;

        for (FTokenSupply tID : tokenIDs)
        {
            jList.Add(tID.GetJson());
        }
        ret.Get()->SetStringField("tokenIDs", UIndexerSupport::jsonObjListToSimpleString(jList));
        return ret;
    }

    /*
    * Used to Handle Edge Cases with Unreal's Json parsing
    */
    void setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>>* lst;
        if (json_in.TryGetArrayField("tokenIDs",lst))
        {
            for (int32 i = 0; i < tokenIDs.Num(); i++)
            {
                const TSharedPtr<FJsonObject>* itemObj;
                if ((*lst)[i].Get()->TryGetObject(itemObj))
                {
                    tokenIDs[i].setup(*itemObj->Get());
                }//if
            }//for
        }//if
    }//setup
};