// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Indexer/Indexer_Enums.h"
#include "SeqEventLog.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqEventLog
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int64 id = -1;
    UPROPERTY()
        TEnumAsByte<EEventLogType> type = EEventLogType::BLOCK_ADDED;
    UPROPERTY()
        int64 blockNumber = -1;
    UPROPERTY()
        FString blockHash = "";
    UPROPERTY()
        FString contractAddress = "";
    UPROPERTY()
        TEnumAsByte<EContractType> contractType = EContractType::ERC1155;
    UPROPERTY()
        FString txnHash = "";
    UPROPERTY()
        int64 txnIndex = -1;
    UPROPERTY()
        int64 txnLogIndex = -1;
    UPROPERTY()
        TEnumAsByte<EEventLogDataType> logDataType = EEventLogDataType::SEQUENCE_TXN;
    UPROPERTY()
        FString ts = "";
    UPROPERTY()
        FString logData = "";
};