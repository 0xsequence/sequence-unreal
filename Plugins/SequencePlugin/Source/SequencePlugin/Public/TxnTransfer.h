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
    {
        TArray<FString> des_keys;
        tokenMetaData.GetKeys(des_keys);
        TMap<FString, TSharedPtr<FJsonValue>> src_data = json_in.Values;
        TArray <FString> src_keys;
        src_data.GetKeys(src_keys);

        for (auto i : src_keys)//once we have the keys we need to compare
        {
            for (auto j : des_keys)
            {//when keys match we inject with the correct data!
                if (i.ToLower().Compare(j.ToLower()) == 0)//we don't care about case here!
                {//if the keys matched inject with the data we need
                    //we need to convert a json value to a json object!
                    tokenMetaData.Find(j)->setup(src_data.Find(i)->Get()->AsObject());
                }//if
            }//for j
        }//for i
    }//setup
};