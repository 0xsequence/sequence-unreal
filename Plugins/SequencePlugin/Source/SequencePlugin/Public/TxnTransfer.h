#pragma once

#include "CoreMinimal.h"
#include "TokenMetaData.h"
#include "Indexer_Enums.h"
#include "TxnTransfer.generated.h"

USTRUCT(BlueprintType)
struct FTxnTransfer
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        TEnumAsByte<ETxnTransferType> transferType;
    UPROPERTY()
        FString contractAddress;
    UPROPERTY()
        TEnumAsByte<EContractType> contractType;
    UPROPERTY()
        FString from;
    UPROPERTY()
        FString to;
    UPROPERTY()
        TArray<int64> tokenIds;
    UPROPERTY()
        TArray<int64> amounts;
    UPROPERTY()
        int32 logIndex;
    UPROPERTY()
        FContractInfo contractInfo;
    UPROPERTY()
        TMap<FString, FTokenMetaData> tokenMetaData;
};