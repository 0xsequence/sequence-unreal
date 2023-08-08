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
        ret += "\"accountAddress\":\"" + accountAddress +"\"";

        if (contractAddress.Len() > 0)
        {
            ret += ",\"contractAddress\":\"" + contractAddress + "\"";
        }

        if (!accountAddresses.IsEmpty())
        {
            ret.Append(",\"accountAddresses\":");
            ret.Append(UIndexerSupport::stringListToSimpleString(accountAddresses));
        }

        if (!contractAddresses.IsEmpty())
        {
            ret.Append(",\"contractAddresses\":" + UIndexerSupport::stringListToSimpleString(contractAddresses));
        }

        if (!transactionHashes.IsEmpty())
        {
            ret.Append(",\"transactionHashes\":");
            ret.Append(UIndexerSupport::stringListToSimpleString(transactionHashes));
        }

        if (!metaTransactionIDs.IsEmpty())
        {
            ret.Append(",\"metaTransactionIDs\":");
            ret.Append(UIndexerSupport::stringListToSimpleString(metaTransactionIDs));
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