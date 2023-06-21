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

    void setup(FJsonObject json_in)
    {
        TArray<TSharedPtr<FJsonValue>> txn_data = json_in.GetArrayField("transactions");

        for (int i = 0; i < transactions.Num(); i++)
        {
            transactions[i].setup(*txn_data[i].Get()->AsObject().Get());
        }
    }
};