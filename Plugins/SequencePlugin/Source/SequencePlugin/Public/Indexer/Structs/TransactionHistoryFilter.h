// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Indexer/IndexerSupport.h"
#include "TransactionHistoryFilter.generated.h"

USTRUCT(BlueprintType)
struct FTransactionHistoryFilter
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
        FString accountAddress = "";
    UPROPERTY(BlueprintReadWrite)
        FString contractAddress = "";
    UPROPERTY(BlueprintReadWrite)
        TArray<FString> accountAddresses;
    UPROPERTY(BlueprintReadWrite)
        TArray<FString> contractAddresses;
    UPROPERTY(BlueprintReadWrite)
        TArray<FString> transactionHashes;
    UPROPERTY(BlueprintReadWrite)
        TArray<FString> metaTransactionIDs;
    UPROPERTY(BlueprintReadWrite)
        int32 fromBlock = -1;
    UPROPERTY(BlueprintReadWrite)
        int32 toBlock = -1;

    /// <summary>
    /// Used to get args formed by this struct in JsonObjectString form
    /// </summary>
    /// <returns>JsonObjectString</returns>
    FString GetArgs()
    {
        FString ret = "{";
        ret += "\"accountAddress\":\"" + accountAddress +"\"";

        if (contractAddress.Len() > 0)
        {
            ret += ",\"contractAddress\":\"" + contractAddress + "\"";
        }

        if (!accountAddresses.IsEmpty())
        {
            ret.Append(",\"accountAddresses\":");
            ret.Append(UIndexerSupport::StringListToParsableString(accountAddresses));
        }

        if (!contractAddresses.IsEmpty())
        {
            ret.Append(",\"contractAddresses\":" + UIndexerSupport::StringListToParsableString(contractAddresses));
        }

        if (!transactionHashes.IsEmpty())
        {
            ret.Append(",\"transactionHashes\":");
            ret.Append(UIndexerSupport::StringListToParsableString(transactionHashes));
        }

        if (!metaTransactionIDs.IsEmpty())
        {
            ret.Append(",\"metaTransactionIDs\":");
            ret.Append(UIndexerSupport::StringListToParsableString(metaTransactionIDs));
        }
        
        if (fromBlock != -1)
        {
            ret.Append(",\"fromBlock\":");
            ret.AppendInt(fromBlock);
        }

        if (toBlock != -1)
        {
            ret.Append(",\"toBlock\":");
            ret.AppendInt(toBlock);
        }

        ret.Append("}");
        return ret;
    }
};