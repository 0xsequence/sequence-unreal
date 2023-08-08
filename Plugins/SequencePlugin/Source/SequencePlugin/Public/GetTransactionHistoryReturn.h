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
    UPROPERTY()
    FPage page;
    UPROPERTY()
        TArray<FTransaction> transactions;
    bool customConstructor = false;//used to tell buildresponse whether or not to use a custom constructor OR the unreal one!
    void construct(FJsonObject json_in) {};//dummy construct for templating

    TSharedPtr<FJsonObject> Get()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetObjectField("page", page.Get());

        TArray<TSharedPtr<FJsonObject>> jList;
        for (FTransaction tItem : transactions)
        {
            jList.Add(tItem.Get());
        }
        ret.Get()->SetStringField("transactions", UIndexerSupport::jsonObjListToSimpleString(jList));
        return ret;
    }

    void setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>>* lst;
        if (json_in.TryGetArrayField("transactions", lst))
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