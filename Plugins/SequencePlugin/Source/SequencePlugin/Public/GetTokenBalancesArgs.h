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

    bool customGetter = false;
    FString Get() { return""; };
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

    bool customGetter = false;
    FString Get() {};
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

     bool customGetter = false;
     FString Get() {};
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

    bool customGetter = false;
    FString Get() {};
};
