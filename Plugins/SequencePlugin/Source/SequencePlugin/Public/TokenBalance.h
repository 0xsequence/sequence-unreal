#pragma once

#include "CoreMinimal.h"
#include "Indexer_Enums.h"
#include "ContractInfo.h"
#include "TokenMetaData.h"
#include "TokenBalance.generated.h"

USTRUCT(BlueprintType)
struct FTokenBalance
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int32 id;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        TEnumAsByte<EContractType> contractType;
    UPROPERTY()
        FString accountAddress;
    UPROPERTY()
        int64 tokenID;
    UPROPERTY()
        int64 balance;
    UPROPERTY()
        FString blockHash;
    UPROPERTY()
        int64 blockNumber;
    UPROPERTY()
        int64 updateID;
    UPROPERTY()
        int64 chainId;
    UPROPERTY()
        FContractInfo contractInfo;
    UPROPERTY()
        FTokenMetaData tokenMetaData;

    void setup(FJsonObject json_in)
    {
        const TSharedPtr<FJsonObject> *item;
        if (json_in.TryGetObjectField("tokenMetaData", item))
        {
            tokenMetaData.setup(*item->Get());
        }
    }//setup
};