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
};