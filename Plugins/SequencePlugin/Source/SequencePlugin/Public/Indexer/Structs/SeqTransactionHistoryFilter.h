
#pragma once

#include "CoreMinimal.h"
#include "Util/SequenceSupport.h"
#include "SeqTransactionHistoryFilter.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqTransactionHistoryFilter
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
        FString accountAddress = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
        FString contractAddress = "";
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
        TArray<FString> accountAddresses;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
        TArray<FString> contractAddresses;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
        TArray<FString> transactionHashes;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
        TArray<FString> metaTransactionIDs;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
        int32 fromBlock = -1;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "0xSequence")
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
            ret.Append(USequenceSupport::StringListToParsableString(accountAddresses));
        }

        if (!contractAddresses.IsEmpty())
        {
            ret.Append(",\"contractAddresses\":" + USequenceSupport::StringListToParsableString(contractAddresses));
        }

        if (!transactionHashes.IsEmpty())
        {
            ret.Append(",\"transactionHashes\":");
            ret.Append(USequenceSupport::StringListToParsableString(transactionHashes));
        }

        if (!metaTransactionIDs.IsEmpty())
        {
            ret.Append(",\"metaTransactionIDs\":");
            ret.Append(USequenceSupport::StringListToParsableString(metaTransactionIDs));
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