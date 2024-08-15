// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Util/SequenceSupport.h"
#include "SeqSortBy.h"
#include "SeqPage.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqPage
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    int32 page = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    FString column = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    FString before = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    FString after = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    TArray<FSeqSortBy> sort;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    int32 pageSize = -1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
    bool more = false;

    /// <summary>
    /// Used to determine if this struct contains ANY NON zeroed data
    /// IE) Arrays => Not Empty, int32 != -1, FString => Not Empty
    /// </summary>
    /// <returns>true if there existed non zero data, else returns false</returns>
    bool containsData()
    {
        bool ret = false;//assume nothing & look for true states!
        ret = (page != -1 || pageSize != -1);//int32 data
        ret = (column.Len() > 0 || before.Len() > 0 || after.Len() > 0);//FString data
        ret = (sort.Num() > 0);//TArray data
        ret |= more;//bool data
        return ret;
    }

    /// <summary>
    /// Used to get custom arguments for cases where we don't need all these datums!
    /// </summary>
    /// <returns>empty string if it doesn't contain non zero data, else it'll contain parsed json string!</returns>
    FString GetArgs()
    {
        FString ret = "";
        if (containsData())
        {
            ret.Append("{");
            ret.Append("\"page\":");
            ret.AppendInt(page);

            if (column.Len() > 0)
                ret += ",\"column\":\""+column+"\"";
            
            if (before.Len() > 0)
                ret += ",\"before\":\""+before+"\"";
            
            if (after.Len() > 0)
                ret += ",\"after\":\"" + after + "\"";

            if (sort.Num() > 0)
            {
                ret.Append(",\"sort\":");
                //convert SortBy to Json!Strings!
                TArray<FString> stringList;
                for (FSeqSortBy sItem : sort)
                {
                    stringList.Add(sItem.GetJsonString());
                }
                //Parse the string list into a JsonString
                ret.Append(USequenceSupport::StringListToSimpleString(stringList));
            }

            if (pageSize != -1)
            {
                ret.Append(",\"pageSize\":");
                ret.AppendInt(pageSize);
            }

            ret.Append(",\"more\":");
            ret.Append(more ? "true" : "false");
            ret.Append("}");
        }
        return ret;
    }

    /*
    * Gets the json Object formed by this struct
    * used for args and testing
    */
    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetNumberField("page", page);
        ret.Get()->SetStringField("column", column);
        ret.Get()->SetStringField("before", before);
        ret.Get()->SetStringField("after", after);

        FString sortString = "[";
        for (FSeqSortBy s : sort)
        {
            sortString.Append(USequenceSupport::SimplifyString(s.GetJsonString()));
            sortString.Append(",");
        }
        if (sort.Num() > 0)
        {
            sortString.RemoveAt(sortString.Len() - 1);
        }
        sortString.Append("]");
        ret.Get()->SetStringField("sort", sortString);
        ret.Get()->SetNumberField("pageSize", pageSize);
        ret.Get()->SetBoolField("more", more);
        return ret;
    }

  
    void setup(FJsonObject json_in){}//dummy method but used for handling edge cases with UE's json parser
};