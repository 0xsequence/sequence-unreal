#pragma once

#include "CoreMinimal.h"
#include "Indexer/Indexer_Enums.h"
#include "SeqContractInfo.h"
#include "SeqTokenMetaData.h"
#include "Dom/JsonObject.h"
#include "SeqTokenBalance.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTokenBalance
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        int32 id = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString contractAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        TEnumAsByte<EContractType> contractType = EContractType::ERC1155;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString accountAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString tokenID = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString balance = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString blockHash = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString blockNumber = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        int64 updateID = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString chainId = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FSeqContractInfo contractInfo;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FSeqTokenMetaData tokenMetaData;

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
        ret.Get()->SetStringField("tokenID", tokenID);
        ret.Get()->SetStringField("balance", balance);
        ret.Get()->SetStringField("blockHash",blockHash);
        ret.Get()->SetStringField("blockNumber", blockNumber);
        ret.Get()->SetNumberField("updateID", updateID);
        ret.Get()->SetStringField("chainId", chainId);
        ret.Get()->SetObjectField("contractInfo", contractInfo.GetJson());
        ret.Get()->SetObjectField("tokenMetaData", tokenMetaData.GetJson());
        return ret;
    };

    /*
    * Used for handling edge cases with unreal's parsing
    */
    void Setup(FJsonObject json_in)
    {
        const TSharedPtr<FJsonObject> *item;
        if (json_in.TryGetObjectField(TEXT("tokenMetaData"), item))
        {
            tokenMetaData.Setup(*item->Get());
        }
    }//setup
};