// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "ContractInfo.h"
#include "TokenMetaData.h"
#include "TokenSupply.generated.h"

USTRUCT(BlueprintType)
struct FTokenSupply
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
        int64 tokenID = -1;
    UPROPERTY(BlueprintReadWrite)
        FString supply = "";
    UPROPERTY(BlueprintReadWrite)
        int64 chainId = -1;
    UPROPERTY(BlueprintReadWrite)
        FContractInfo contractInfo;
    UPROPERTY(BlueprintReadWrite)
        FTokenMetaData tokenMetaData;

    /*
    * Used to get the jsonobject formed by the struct
    * used in testing & args
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetNumberField("tokenID",tokenID);
        ret.Get()->SetStringField("supply", supply);
        ret.Get()->SetNumberField("chainId", chainId);
        ret.Get()->SetObjectField("contractInfo", contractInfo.GetJson());
        ret.Get()->SetObjectField("tokenMetaData", tokenMetaData.GetJson());
        return ret;
    }

    /*
    * Used to handle edge case parsing with unreal's json parser
    */
    void setup(FJsonObject json_in)
    {
        const TSharedPtr<FJsonObject>* itemObj;
        if (json_in.TryGetObjectField(TEXT("tokenMetaData"), itemObj))
        {
            tokenMetaData.setup(*itemObj->Get());
        }
    }
};