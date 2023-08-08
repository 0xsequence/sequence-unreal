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

    TSharedPtr<FJsonObject> Get()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetStringField("txnHash", txnHash);
        ret.Get()->SetNumberField("blockNumber", blockNumber);
        ret.Get()->SetStringField("blockHash", blockHash);
        ret.Get()->SetNumberField("chainId", chainId);
        ret.Get()->SetStringField("metaTxnID", metaTxnID);

        TArray<TSharedPtr<FJsonObject>> jList;

        for (FTxnTransfer tItem : transfers)
        {
            jList.Add(tItem.Get());
        }

        ret.Get()->SetStringField("transfers", UIndexerSupport::jsonObjListToSimpleString(jList));

        ret.Get()->SetStringField("timestamp", timestamp);
        return ret;
    }

    void setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>>* trnsfrs;
        if (json_in.TryGetArrayField("transfers", trnsfrs))
        {
            //here we know the array's are gonna be parallel as we setup up the UStruct with the exact same data!
            for (int i = 0; i < transfers.Num(); i++)
            {
                const TSharedPtr<FJsonObject>* itemObj;
                if ((*trnsfrs)[i].Get()->TryGetObject(itemObj))
                {
                    transfers[i].setup(*itemObj->Get());
                }
            }
        }
    }
};
