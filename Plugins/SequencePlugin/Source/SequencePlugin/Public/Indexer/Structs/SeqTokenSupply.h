// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqContractInfo.h"
#include "SeqTokenMetaData.h"
#include "SeqTokenSupply.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTokenSupply
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        int64 tokenID = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString supply = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        int64 chainId = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FSeqContractInfo contractInfo;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FSeqTokenMetaData tokenMetaData;

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
            tokenMetaData.Setup(*itemObj->Get());
        }
    }
};