#pragma once

#include "CoreMinimal.h"
#include "Transaction.generated.h"

USTRUCT(BlueprintType)
struct FTransaction
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString txnHash;
    UPROPERTY()
        int64 blockNumber;
    UPROPERTY()
        FString blockHash;
    UPROPERTY()
        int64 chainId;
    UPROPERTY()
        FString metaTxnID;
    UPROPERTY()
        TArray<FTxnTransfer> transfers;
    UPROPERTY()
        FString timestamp;
};
