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

    TSharedPtr<FJsonObject> Get()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetNumberField("id", id);
        ret.Get()->SetStringField("contractAddress", contractAddress);
        ret.Get()->SetStringField("contractType", UEnum::GetValueAsString(contractType.GetValue()));
        ret.Get()->SetStringField("accountAddress", accountAddress);
        ret.Get()->SetNumberField("tokenID", tokenID);
        ret.Get()->SetNumberField("balance", balance);
        ret.Get()->SetStringField("blockHash",blockHash);
        ret.Get()->SetNumberField("blockNumber", blockNumber);
        ret.Get()->SetNumberField("updateID", updateID);
        ret.Get()->SetNumberField("chainId", chainId);
        ret.Get()->SetObjectField("contractInfo", contractInfo.Get());
        ret.Get()->SetObjectField("tokenMetaData", tokenMetaData.Get());
        return ret;
    };

    void setup(FJsonObject json_in)
    {
        const TSharedPtr<FJsonObject> *item;
        if (json_in.TryGetObjectField("tokenMetaData", item))
        {
            tokenMetaData.setup(*item->Get());
        }
    }//setup
};