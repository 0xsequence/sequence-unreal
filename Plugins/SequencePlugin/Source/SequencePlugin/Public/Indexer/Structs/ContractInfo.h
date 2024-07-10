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
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int64 chainId = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString address = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString name = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString type = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString symbol = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 decimals = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString logoURI = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
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