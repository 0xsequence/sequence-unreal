// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqPage.h"
#include "SeqTokenBalance.h"
#include "SeqGetBalanceUpdatesReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetBalanceUpdatesReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FSeqPage page;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TArray<FSeqTokenBalance> balances;
    //we don't need custom constructor for this!
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating

    /*
    * Gets the jsonObject formed by this struct
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetObjectField("page", page.GetJson());
        TArray<TSharedPtr<FJsonObject>> jsonList;

        for (FSeqTokenBalance tItem : balances)
        {
            jsonList.Add(tItem.GetJson());
        }
        ret.Get()->SetStringField("balances", USequenceSupport::JsonObjListToSimpleString(jsonList));

        return ret;
    }

    /*
    * Used to handle edge cases with Unreal's json parsing
    */
    void Setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>> *lst;
        if (json_in.TryGetArrayField(TEXT("balances"), lst))//verify it's an arrayfield and valid!
        {
            for (int32 i = 0; i < balances.Num(); i++)
            {
                const TSharedPtr<FJsonObject> *itemObj;
                if ((*lst)[i].Get()->TryGetObject(itemObj))//verify that there's an object here!
                {
                    balances[i].Setup(*itemObj->Get());
                }
            }
        }
    }
};