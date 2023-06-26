#pragma once

#include "CoreMinimal.h"
#include "TxnTransfer.h"
#include "Transaction.generated.h"

USTRUCT(BlueprintType)
struct FTransaction
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString txnHash;
    UPROPERTY()
        int64 blockNumber;
    UPROPERTY()
        FString blockHash;
    UPROPERTY()
        int64 chainId;
    UPROPERTY()
        FString metaTxnID;
    UPROPERTY()
        TArray<FTxnTransfer> transfers;
    UPROPERTY()
        FString timestamp;

    void setup(FJsonObject json_in)
    {
        TArray<TSharedPtr<FJsonValue>> trnsfrs = json_in.GetArrayField("transfers");

        //here we know the array's are gonna be parallel as we setup up the UStruct with the exact same data!
        for (int i = 0; i < transfers.Num(); i++)
        {
            transfers[i].setup(*trnsfrs[i].Get()->AsObject().Get());
        }

    }
};
