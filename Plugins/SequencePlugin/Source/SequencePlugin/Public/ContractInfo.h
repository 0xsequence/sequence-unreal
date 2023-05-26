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
        int64 chainID;
    UPROPERTY()
        FString address;
    UPROPERTY()
        FString name;
    UPROPERTY()
        FString type;
    UPROPERTY()
        FString symbol;
    UPROPERTY()
        int32 decimals;
    UPROPERTY()
        FString logoURI;
    UPROPERTY()
        FContractInfoExtensions extensions;
};