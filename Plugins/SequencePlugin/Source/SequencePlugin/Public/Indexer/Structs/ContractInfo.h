// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "ContractInfoExtensions.h"
#include "ContractInfo.generated.h"

USTRUCT(BlueprintType)
struct FContractInfo
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
        int64 chainId = -1;
    UPROPERTY(BlueprintReadWrite)
        FString address = "";
    UPROPERTY(BlueprintReadWrite)
        FString name = "";
    UPROPERTY(BlueprintReadWrite)
        FString type = "";
    UPROPERTY(BlueprintReadWrite)
        FString symbol = "";
    UPROPERTY(BlueprintReadWrite)
        int32 decimals = -1;
    UPROPERTY(BlueprintReadWrite)
        FString logoURI = "";
    UPROPERTY(BlueprintReadWrite)
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