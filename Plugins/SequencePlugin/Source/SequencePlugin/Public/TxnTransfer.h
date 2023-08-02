#pragma once

#include "CoreMinimal.h"
#include "TokenMetaData.h"
#include "Indexer_Enums.h"
#include "ContractInfo.h"
#include "TxnTransfer.generated.h"


USTRUCT(BlueprintType)
struct FTxnTransfer
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        TEnumAsByte<ETxnTransferType> transferType;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        TEnumAsByte<EContractType> contractType;
    UPROPERTY()
        FString from;
    UPROPERTY()
        FString to;
    UPROPERTY()
        TArray<int64> tokenIds;
    UPROPERTY()
        TArray<int64> amounts;
    UPROPERTY()
        int32 logIndex;
    UPROPERTY()
        FContractInfo contractInfo;
    UPROPERTY()
        TMap<FString, FTokenMetaData> tokenMetaData;

    void setup(FJsonObject json_in)
    {//the json object we get will be a mirror to this!        
        const TSharedPtr<FJsonObject>* ptrJson;
        if (json_in.TryGetObjectField("tokenMetaData",ptrJson))
        {
            TArray<FString> srcKeys;
            ptrJson->Get()->Values.GetKeys(srcKeys);

            for (FString i : srcKeys)//go through src keys and update what does exist!
            {
                if (tokenMetaData.Contains(i))
                {//key exists!
                    TSharedPtr<FJsonValue> * srcEntry = ptrJson->Get()->Values.Find(i);
                    const TSharedPtr<FJsonObject>* srcEntryPtr;
                    if (srcEntry->Get()->TryGetObject(srcEntryPtr))
                    {
                        tokenMetaData.Find(i)->setup(*srcEntryPtr->Get());
                    }//valid src Object
                }//Matching Key!
            }//For loop through all source keys
        }//we have tokenMetaData to parse!
    }//setup
};