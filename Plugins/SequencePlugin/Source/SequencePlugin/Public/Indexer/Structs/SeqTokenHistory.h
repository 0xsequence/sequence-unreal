#pragma once

#include "CoreMinimal.h"
#include "Indexer/Indexer_Enums.h"
#include "SeqTokenHistory.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTokenHistory
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