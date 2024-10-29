// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqTxnTransfer.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "SeqTransaction.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransaction
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString txnHash = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        int64 blockNumber = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString blockHash = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        int64 chainId = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString metaTxnID = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TArray<FSeqTxnTransfer> transfers;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
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

        for (FSeqTxnTransfer tItem : transfers)
        {
            jList.Add(tItem.GetJson());
        }

        ret.Get()->SetStringField("transfers", USequenceSupport::JsonObjListToSimpleString(jList));

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
