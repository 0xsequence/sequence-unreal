#pragma once

#include "CoreMinimal.h"
#include "TransactionHistoryFilter.generated.h"

USTRUCT(BlueprintType)
struct FTransactionHistoryFilter
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString accountAddress = "";
    UPROPERTY()
        FString contractAddress = "";
    UPROPERTY()
        TArray<FString> accountAddresses;
    UPROPERTY()
        TArray<FString> contractAddresses;
    UPROPERTY()
        TArray<FString> transactionHashes;
    UPROPERTY()
        TArray<FString> metaTransactionIDs;
    UPROPERTY()
        int32 fromBlock = -1;
    UPROPERTY()
        int32 toBlock = -1;

    FString GetArgs()
    {
        FString ret = "{";
        ret += "\"accountAddress\":" + accountAddress;//MUST HAVE THIS!

        //everything below is optional so if we don't have anything there it won't be parsed in!
        if (!contractAddress.IsEmpty())
            ret += ",\"contractAddress\":" + contractAddress;

        if (!accountAddress.IsEmpty())
            ret += ",\"accountAddresses\":" + UIndexerSupport::stringListToSimpleString(accountAddresses);

        if (!contractAddresses.IsEmpty())
            ret += ",\"contractAddresses\":" + UIndexerSupport::stringListToSimpleString(contractAddresses);

        if (!transactionHashes.IsEmpty())
            ret += ",\"transactionHashes\":" + UIndexerSupport::stringListToSimpleString(transactionHashes);

        if (!metaTransactionIDs.IsEmpty())
            ret += ",\"metaTransactionIDs\":" + UIndexerSupport::stringListToSimpleString(metaTransactionIDs);
        
        if (fromBlock != -1)
            ret += ",\"fromBlock\":" + fromBlock;

        if (toBlock != -1)
            ret += ",\"toBlock\":" + toBlock;

        ret += "}";
        return ret;
    }
};