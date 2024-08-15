// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Indexer/Indexer_Enums.h"
#include "TokenHistory.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FTokenHistory
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        int32 id = -1;
    UPROPERTY()
        int64 blockNumber = -1;
    UPROPERTY()
        FString blockHash = "";
    UPROPERTY()
        FString contractAddress = "";
    UPROPERTY()
        TEnumAsByte<EContractType> contractType = EContractType::ERC1155;
    UPROPERTY()
        FString fromAddress = "";
    UPROPERTY()
        FString toAddress = "";
    UPROPERTY()
        FString txnHash = "";
    UPROPERTY()
        int32 txnIndex = -1;
    UPROPERTY()
        int32 txnLogIndex = -1;
    UPROPERTY()
        FString logData = "";
    UPROPERTY()
        FString ts = "";
};