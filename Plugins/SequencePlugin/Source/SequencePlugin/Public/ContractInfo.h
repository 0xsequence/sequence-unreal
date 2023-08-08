#pragma once

#include "CoreMinimal.h"
#include "ContractInfoExtensions.h"
#include "ContractInfo.generated.h"

USTRUCT(BlueprintType)
struct FContractInfo
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int64 chainId = -1;
    UPROPERTY()
        FString address = "";
    UPROPERTY()
        FString name = "";
    UPROPERTY()
        FString type = "";
    UPROPERTY()
        FString symbol = "";
    UPROPERTY()
        int32 decimals = -1;
    UPROPERTY()
        FString logoURI = "";
    UPROPERTY()
        FContractInfoExtensions extensions;

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