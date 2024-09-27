#pragma once

#include "CoreMinimal.h"
#include "SeqMarketplaceSortBy.h"  // Ensure this includes your SortBy struct or enum
#include "SeqMarketplacePage.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqMarketplacePage
{
    GENERATED_USTRUCT_BODY()

public:
    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    int32 PageNumber = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    FString Column;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    FString Before;  // Assuming Before and After are strings, since Unreal doesn't support generic 'object' types

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    FString After;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    TArray<FSeqMarketplaceSortBy> Sort;  // Assuming you have a SortBy struct or enum

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    int32 PageSize = 30;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    bool More;

    bool ContainsData()
    {
        bool ret = false;//assume nothing & look for true states!
        ret = (PageNumber != -1 || PageSize != -1);//int32 data
        ret = (Column.Len() > 0 || Before.Len() > 0 || After.Len() > 0);//FString data
        ret = (Sort.Num() > 0);//TArray data
        ret |= More;//bool data
        return ret;
    }

    FString GetArgs()
    {
        FString ret = "";
        if (ContainsData())
        {
            ret.Append("{");
            ret.Append("\"page\":");
            ret.AppendInt(PageNumber);

            if (Column.Len() > 0)
                ret += ",\"column\":\"" + Column + "\"";

            if (Before.Len() > 0)
                ret += ",\"before\":\"" + Before + "\"";

            if (After.Len() > 0)
                ret += ",\"after\":\"" + After + "\"";

            if (Sort.Num() > 0)
            {
                ret.Append(",\"sort\":");
                //convert SortBy to Json!Strings!
                TArray<FString> stringList;
                for (FSeqMarketplaceSortBy sItem : Sort)
                {
                    stringList.Add(sItem.GetJsonString());
                }
                //Parse the string list into a JsonString
                ret.Append(USequenceSupport::StringListToSimpleString(stringList));
            }

            if (PageSize != -1)
            {
                ret.Append(",\"pageSize\":");
                ret.AppendInt(PageSize);
            }

            ret.Append(",\"more\":");
            ret.Append(More ? "true" : "false");
            ret.Append("}");
        }
        return ret;
    }


    TSharedPtr<FJsonObject> GetJson()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetNumberField("page", PageNumber);
        ret.Get()->SetStringField("column", Column);
        ret.Get()->SetStringField("before", Before);
        ret.Get()->SetStringField("after", After);

        FString sortString = "[";
        for (FSeqMarketplaceSortBy s : Sort)
        {
            sortString.Append(USequenceSupport::SimplifyString(s.GetJsonString()));
            sortString.Append(",");
        }
        if (Sort.Num() > 0)
        {
            sortString.RemoveAt(sortString.Len() - 1);
        }
        sortString.Append("]");
        ret.Get()->SetStringField("sort", sortString);
        ret.Get()->SetNumberField("pageSize", PageSize);
        ret.Get()->SetBoolField("more", More); 
        return ret;
    }
    void setup(FJsonObject json_in) {}

};