// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqPage.h"
#include "SeqTransactionHistoryFilter.h"
#include "Dom/JsonObject.h"
#include "SeqGetTransactionHistoryArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetTransactionHistoryArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FSeqTransactionHistoryFilter filter;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TOptional<FSeqPage> page;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        bool includeMetaData = false;

    bool customGetter = true;
    FString GetArgs()
    {
        FString ret = "{";
        ret.Append("\"filter\":");
        ret.Append(filter.GetArgs());//get the args! MUST Have this!
        if (page.IsSet())
        {
            ret.Append(".\"page\":");
            ret.Append(page.GetValue().GetArgs());
            TSharedPtr<FJsonObject> PageObject = MakeShared<FJsonObject>();
            FJsonObjectConverter::UStructToJsonObject(FSeqPage::StaticStruct(), &(page.GetValue()), PageObject.ToSharedRef(), 0, 0);
            
        }

        ret.Append(",\"includeMetaData\":");
        ret.Append(includeMetaData ? "true" : "false");

        ret.Append("}");

        return ret;
    }

    FSeqGetTransactionHistoryArgs() {};

    FSeqGetTransactionHistoryArgs(FString addr_in)
    {
        FSeqTransactionHistoryFilter filter_data;
        filter_data.accountAddress = addr_in;
        this->filter = filter_data;
    };
};