// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Indexer/Indexer_Enums.h"
#include "ContractInfo.h"
#include "TokenMetaData.h"
#include "TokenBalance.generated.h"

USTRUCT(BlueprintType)
struct FTokenBalance
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int32 id = -1;
    UPROPERTY()
        FString contractAddress = "";
    UPROPERTY()
        TEnumAsByte<EContractType> contractType = EContractType::ERC1155;
    UPROPERTY()
        FString accountAddress = "";
    UPROPERTY()
        int64 tokenID = -1;
    UPROPERTY()
        int64 balance = -1;
    UPROPERTY()
        FString blockHash = "";
    UPROPERTY()
        int64 blockNumber = -1;
    UPROPERTY()
        int64 updateID = -1;
    UPROPERTY()
        int64 chainId = -1;
    UPROPERTY()
        FContractInfo contractInfo;
    UPROPERTY()
        FTokenMetaData tokenMetaData;

    /*
    * Used to get the json object formed by this struct
    * used for testing and args
    */
    TSharedPtr<FJsonObject> GetJson()
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
        ret.Get()->SetObjectField("contractInfo", contractInfo.GetJson());
        ret.Get()->SetObjectField("tokenMetaData", tokenMetaData.GetJson());
        return ret;
    };

    /*
    * Used for handling edge cases with unreal's parsing
    */
    void setup(FJsonObject json_in)
    {
        const TSharedPtr<FJsonObject> *item;
        if (json_in.TryGetObjectField("tokenMetaData", item))
        {
            tokenMetaData.setup(*item->Get());
        }
    }//setup
};