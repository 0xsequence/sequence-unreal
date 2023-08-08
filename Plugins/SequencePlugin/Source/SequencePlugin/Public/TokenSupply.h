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
    UPROPERTY()
        int64 tokenID;
    UPROPERTY()
        FString supply;
    UPROPERTY()
        int64 chainId;
    UPROPERTY()
        FContractInfo contractInfo;
    UPROPERTY()
        FTokenMetaData tokenMetaData;

    TSharedPtr<FJsonObject> Get()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetNumberField("tokenID",tokenID);
        ret.Get()->SetStringField("supply", supply);
        ret.Get()->SetNumberField("chainId", chainId);
        ret.Get()->SetObjectField("contractInfo", contractInfo.Get());
        ret.Get()->SetObjectField("tokenMetaData", tokenMetaData.Get());
        return ret;
    }

    void setup(FJsonObject json_in)
    {
        const TSharedPtr<FJsonObject>* itemObj;
        if (json_in.TryGetObjectField("tokenMetaData", itemObj))
        {
            tokenMetaData.setup(*itemObj->Get());
        }
    }
};