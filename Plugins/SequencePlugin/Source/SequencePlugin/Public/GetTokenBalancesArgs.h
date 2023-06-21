#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "GetTokenBalancesArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenBalancesArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString accountAddress;
};

USTRUCT(BlueprintType)
struct FGetTokenBalancesArgs_AC
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString accountAddress;
    UPROPERTY()
        FString contractAddress;
};

USTRUCT(BlueprintType)
struct FGetTokenBalancesArgs_ACI
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString accountAddress;
    UPROPERTY()
         FString contractAddress;
     UPROPERTY()
         bool includeMetaData;
};

USTRUCT(BlueprintType)
struct FGetTokenBalancesArgs_ACIP
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString accountAddress;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        bool includeMetaData;
    UPROPERTY()
        FPage page;
};
