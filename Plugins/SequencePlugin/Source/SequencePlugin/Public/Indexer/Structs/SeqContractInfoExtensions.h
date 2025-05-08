
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "SeqContractInfoExtensions.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqContractInfoExtensions
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString link = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString description = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString ogImage = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        int32 originChainId = -1;//might be int64?
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString originAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        bool blacklist = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        bool verified = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString verifiedBy = "";

    /*
    * Gets the json object formed by this struct
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetStringField("link", link);
        ret.Get()->SetStringField("description", description);
        ret.Get()->SetStringField("ogImage", ogImage);
        ret.Get()->SetNumberField("originChainId", originChainId);
        ret.Get()->SetStringField("originAddress", originAddress);
        ret.Get()->SetBoolField("blacklist", blacklist);
        ret.Get()->SetBoolField("verified",verified);
        ret.Get()->SetStringField("verifiedBy",verifiedBy);
        return ret;
    };
};