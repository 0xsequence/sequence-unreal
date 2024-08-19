// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqTokenMetaData.h"
#include "Indexer/Indexer_Enums.h"
#include "SeqContractInfo.h"
#include "SeqTxnTransfer.generated.h"


USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTxnTransfer
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TEnumAsByte<ETxnTransferType> transferType = ETxnTransferType::RECEIVE;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString contractAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TEnumAsByte<EContractType> contractType = EContractType::ERC1155;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString from = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString to = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TArray<int64> tokenIds;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TArray<int64> amounts;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        int32 logIndex = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FSeqContractInfo contractInfo;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TMap<FString, FSeqTokenMetaData> tokenMetaData;

    /*
    * Used to return the Json Object formed by this struct
    * used for args and testing
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);

        ret.Get()->SetStringField("transferType", UEnum::GetValueAsString(transferType.GetValue()));
        ret.Get()->SetStringField("contractAddress", contractAddress);
        ret.Get()->SetStringField("contractType", UEnum::GetValueAsString(contractType.GetValue()));
        ret.Get()->SetStringField("from",from);
        ret.Get()->SetStringField("to", to);
        ret.Get()->SetStringField("tokenIds",USequenceSupport::Int64ListToSimpleString(tokenIds));
        ret.Get()->SetStringField("amounts", USequenceSupport::Int64ListToSimpleString(amounts));
        ret.Get()->SetNumberField("logIndex", logIndex);
        ret.Get()->SetObjectField("contractInfo", contractInfo.GetJson());
        TSharedPtr<FJsonObject> nRet = MakeShareable<FJsonObject>(new FJsonObject);
        TArray<FString> keys;
        tokenMetaData.GetKeys(keys);
        for (FString key : keys)
        {
            TSharedPtr<FJsonObject> value = tokenMetaData.Find(key)->GetJson();
            nRet.Get()->SetObjectField(key,value);
        }

        ret.Get()->SetObjectField("tokenMetaData",nRet);

        return ret;
    }

    /*
    * Used to Handle Edge Cases with Unreal's Json parsing
    */
    void setup(FJsonObject json_in)
    {//the json object we get will be a mirror to this!        
        const TSharedPtr<FJsonObject>* ptrJson;
        if (json_in.TryGetObjectField(TEXT("tokenMetaData"),ptrJson))
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