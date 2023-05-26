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
        TEnumAsByte<EEventLogType> type;
    UPROPERTY()
        int64 blockNumber;
    UPROPERTY()
        FString blockHash;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        TEnumAsByte<EContractType> contractType;
    UPROPERTY()
        FString txnHash;
    UPROPERTY()
        int64 txnIndex;
    UPROPERTY()
        int64 txnLogIndex;
    UPROPERTY()
        TEnumAsByte<EEventLogDataType> logDataType;
    UPROPERTY()
        FString ts;
    UPROPERTY()
        FString logData;
};