
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "SeqContractInfoExtensions.h"
#include "SeqContractInfo.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqContractInfo
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        int64 chainId = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString address = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString name = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString type = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString symbol = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        int32 decimals = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString logoURI = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FSeqContractInfoExtensions extensions;

    /*
    * Gets the json object formed by this struct
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetNumberField("chainId", chainId);
        ret.Get()->SetStringField("address", address);
        ret.Get()->SetStringField("name", name);
        ret.Get()->SetStringField("type", type);
        ret.Get()->SetStringField("symbol", symbol);
        ret.Get()->SetNumberField("decimals", decimals);
        ret.Get()->SetStringField("logoURI", logoURI);
        ret.Get()->SetObjectField("extensions", extensions.GetJson());
        return ret;
    }
};