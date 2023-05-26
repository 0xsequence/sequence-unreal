#pragma once

#include "CoreMinimal.h"
#include "Indexer_Enums.h"
#include "EventLog.generated.h"

USTRUCT(BlueprintType)
struct FEventLog
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int64 id;
    UPROPERTY()
        EventLogType type;
    UPROPERTY()
        int64 blockNumber;
    UPROPERTY()
        FString blockHash;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        ContractType contractType;
    UPROPERTY()
        FString txnHash;
    UPROPERTY()
        int64 txnIndex;
    UPROPERTY()
        int64 txnLogIndex;
    UPROPERTY()
        EventLogDataType logDataType;
    UPROPERTY()
        FString ts;
    UPROPERTY()
        FString logData;
};