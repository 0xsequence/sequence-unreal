// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "TxnTransfer.h"
#include "Transaction.generated.h"

USTRUCT(BlueprintType)
struct FTransaction
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString txnHash = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int64 blockNumber = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString blockHash = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int64 chainId = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString metaTxnID = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<FTxnTransfer> transfers;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString timestamp = "";

    /*
    * Used to get the json Object formed by this struct
    * used for args & testing
    */
    TSharedPtr<FJsonObject> GetJson()
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
            jList.Add(tItem.GetJson());
        }

        ret.Get()->SetStringField("transfers", UIndexerSupport::JsonObjListToSimpleString(jList));

        ret.Get()->SetStringField("timestamp", timestamp);
        return ret;
    }

    /*
    * Used to handle edge case parsing with Unreal's json parser
    */
    void setup(FJsonObject json_in)
    {
        const TArray<TSharedPtr<FJsonValue>>* trnsfrs;
        if (json_in.TryGetArrayField(TEXT("transfers"), trnsfrs))
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
