#pragma once

#include "CoreMinimal.h"
#include "TransactionHistoryFilter.generated.h"

USTRUCT(BlueprintType)
struct FTransactionHistoryFilter
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString accountAddress;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        TArray<FString> accountAddresses;
    UPROPERTY()
        TArray<FString> contractAddresses;
    UPROPERTY()
        TArray<FString> transactionHashes;
    UPROPERTY()
        TArray<FString> metaTransactionIDs;
    UPROPERTY()
        int32 fromBlock;
    UPROPERTY()
        int32 toBlock;
};