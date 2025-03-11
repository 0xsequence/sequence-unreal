// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqPage.h"
#include "Util/SequenceSupport.h"
#include "SeqTokenBalance.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "SeqGetTokenBalancesReturn.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetTokenBalancesReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FSeqPage page;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        TArray<FSeqTokenBalance> balances;
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating

    /*
    * Used to return a json Object for arg's purposes / testing
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);

        ret.Get()->SetObjectField("page", page.GetJson());
        TArray<TSharedPtr<FJsonObject>> balancesList;
        for (FSeqTokenBalance tBalance : balances)
        {
            balancesList.Add(tBalance.GetJson());
        }
        ret.Get()->SetStringField("balances",USequenceSupport::JsonObjListToSimpleString(balancesList));
        return ret;
    }

    /*
    * Used to Handle Edge Cases with Unreal's Json parsing
    */
    void Setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>>* lst;
        if (json_in.TryGetArrayField(TEXT("balances"), lst))
        {
            for (int32 i = 0; i < balances.Num(); i++)
            {
                TSharedPtr<FJsonValue> item = (*lst)[i];
                const TSharedPtr<FJsonObject>* itemObj;
                if ((*lst)[i].Get()->TryGetObject(itemObj))
                {
                    balances[i].Setup(*itemObj->Get());
                }
            }
        }
    }
};