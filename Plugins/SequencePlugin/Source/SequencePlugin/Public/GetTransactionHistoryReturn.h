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

    FGetTransactionHistoryReturn() {};

    FGetTransactionHistoryReturn(FJsonObject json_in)
    {
        TSharedPtr<FJsonObject> page_obj = json_in.GetObjectField("page");
        //assign the data to page!
        FJsonObjectConverter::JsonObjectToUStruct<FPage>(page_obj.ToSharedRef(), &page);

        TArray<TSharedPtr<FJsonValue>> txn_list = json_in.GetArrayField("transactions");

        for (auto i : txn_list)
        {
            FTransaction txn;
            txn = FTransaction();
            txn.blockHash = i.Get()->AsObject().Get()->GetStringField("blockHash");
            txn.blockNumber = i.Get()->AsObject().Get()->GetIntegerField("blockNumber");
            txn.chainId = i.Get()->AsObject().Get()->GetIntegerField("chainId");
            txn.metaTxnID = i.Get()->AsObject().Get()->GetStringField("metaTxnID");
            txn.timestamp = i.Get()->AsObject().Get()->GetStringField("timeStamp");
            txn.txnHash = i.Get()->AsObject().Get()->GetStringField("txnHash");
            
            TArray<TSharedPtr<FJsonValue>> trns_frs = i.Get()->AsObject().Get()->GetArrayField("transfers");

            

            for (auto j : trns_frs)
            {
                
            }
        }

    };
};