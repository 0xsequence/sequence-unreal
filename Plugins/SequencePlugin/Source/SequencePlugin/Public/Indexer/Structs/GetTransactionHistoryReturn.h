// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "Transaction.h"
#include "GetTransactionHistoryReturn.generated.h"

USTRUCT(BlueprintType)
struct FGetTransactionHistoryReturn
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    FPage page;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TArray<FTransaction> transactions;
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating

    /*
    * Used to get the json object formed by this struct, used for
    * args & testing purposes
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetObjectField("page", page.GetJson());

        TArray<TSharedPtr<FJsonObject>> jList;
        for (FTransaction tItem : transactions)
        {
            jList.Add(tItem.GetJson());
        }
        ret.Get()->SetStringField("transactions", USequenceSupport::JsonObjListToSimpleString(jList));
        return ret;
    }

    /*
    * Used to Handle Edge Cases with Unreal's Json parsing
    */
    void Setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>>* lst;
        if (json_in.TryGetArrayField(TEXT("transactions"), lst))
        {
            for (int i = 0; i < transactions.Num(); i++)
            {
                const TSharedPtr<FJsonObject>* itemObj;
                if ((*lst)[i].Get()->TryGetObject(itemObj))
                {
                    transactions[i].setup(*itemObj->Get());
                }//if
            }//for
        }//if
    }//setup
};