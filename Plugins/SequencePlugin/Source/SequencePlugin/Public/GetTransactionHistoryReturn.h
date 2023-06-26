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
    void setup(FJsonObject json_in)
    {
        TArray<TSharedPtr<FJsonValue>> txn_data = json_in.GetArrayField("transactions");

        for (int i = 0; i < transactions.Num(); i++)
        {
            transactions[i].setup(*txn_data[i].Get()->AsObject().Get());
        }
    }
};