#pragma once

#include "CoreMinimal.h"
#include "Indexer_Enums.h"
#include "TokenHistory.generated.h"

USTRUCT(BlueprintType)
struct FTokenHistory
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int32 id;
    UPROPERTY()
        int64 blockNumber;
    UPROPERTY()
        FString blockHash;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        ContractType contractType;
    UPROPERTY()
        FString fromAddress;
    UPROPERTY()
        FString toAddress;
    UPROPERTY()
        FString txnHash;
    UPROPERTY()
        int32 txnIndex;
    UPROPERTY()
        int32 txnLogIndex;
    UPROPERTY()
        FString logData;
    UPROPERTY()
        FString ts;
};