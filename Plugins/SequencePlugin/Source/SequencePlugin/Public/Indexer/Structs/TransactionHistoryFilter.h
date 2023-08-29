#pragma once

#include "CoreMinimal.h"
#include "Indexer/IndexerSupport.h"
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
            ret.Append(UIndexerSupport::stringListToParsableString(accountAddresses));
        }

        if (!contractAddresses.IsEmpty())
        {
            ret.Append(",\"contractAddresses\":" + UIndexerSupport::stringListToParsableString(contractAddresses));
        }

        if (!transactionHashes.IsEmpty())
        {
            ret.Append(",\"transactionHashes\":");
            ret.Append(UIndexerSupport::stringListToParsableString(transactionHashes));
        }

        if (!metaTransactionIDs.IsEmpty())
        {
            ret.Append(",\"metaTransactionIDs\":");
            ret.Append(UIndexerSupport::stringListToParsableString(metaTransactionIDs));
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