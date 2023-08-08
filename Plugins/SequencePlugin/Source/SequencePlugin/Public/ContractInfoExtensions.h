#pragma once

#include "CoreMinimal.h"
#include "ContractInfoExtensions.generated.h"

USTRUCT(BlueprintType)
struct FContractInfoExtensions
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString link;
    UPROPERTY()
        FString description;
    UPROPERTY()
        FString ogImage;
    UPROPERTY()
        int32 originChainId;//might be int64?
    UPROPERTY()
        FString originAddress;
    UPROPERTY()
        bool blacklist;

    //for testing
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetStringField("link", link);
        ret.Get()->SetStringField("description", description);
        ret.Get()->SetStringField("ogImage", ogImage);
        ret.Get()->SetNumberField("originChainId", originChainId);
        ret.Get()->SetStringField("originAddress", originAddress);
        ret.Get()->SetBoolField("blacklist", blacklist);
        return ret;
    };
};