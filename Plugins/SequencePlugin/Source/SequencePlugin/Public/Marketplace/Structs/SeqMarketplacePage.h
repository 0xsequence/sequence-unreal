#pragma once

#include "CoreMinimal.h"
#include "SeqMarketplaceSortBy.h"  // Ensure this includes your SortBy struct or enum
#include "Util/JsonBuilder.h"
#include "Util/SequenceSupport.h"
#include "SeqMarketplacePage.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqMarketplacePage
{
    GENERATED_USTRUCT_BODY()

public:
    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    int32 PageNumber =-1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    FString Column;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    FString Before;  // Assuming Before and After are strings, since Unreal doesn't support generic 'object' types

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    FString After;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    TArray<FSeqMarketplaceSortBy> Sort;  // Assuming you have a SortBy struct or enum

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    int32 PageSize =-1 ;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    bool More;

    static FSeqMarketplacePage Empty()
    {
        return FSeqMarketplacePage{
        };
    }

    static FSeqMarketplacePage First()
    {
        return FSeqMarketplacePage{
        1
        };
    }
 
    void IncrementPage()
    {
        PageNumber += 1;
    }

    bool ContainsData()
    {
        bool ret = false;//assume nothing & look for true states!
        ret |= (PageNumber != -1 || PageSize != -1);//int32 data
        ret |= (Column.Len() > 0 || Before.Len() > 0 || After.Len() > 0);//FString data
        ret |= (Sort.Num() > 0);//TArray data
        ret |= More;//bool data
        return ret;
    }

    FString GetArgs()
    {
        FJsonBuilder Json;
        
        if (ContainsData())
        {
            if (PageNumber != -1)
            {
                Json.AddInt("page", PageNumber);
            }

            if (Column.Len() > 0) Json.AddString("column", Column);
            if (Before.Len() > 0) Json.AddString("before", Before);
            if (Sort.Num() > 0)
            {
                auto Array = Json.AddArray("sort");

                for(FSeqMarketplaceSortBy SortBy : Sort)
                {
                    Array.AddString(SortBy.GetJsonString());
                }

                Json = *Array.EndArray();
            }

            if (PageSize != -1)
            {
                Json.AddInt("pageSize", PageSize);
            }

            Json.AddBool("more", More);
        }
        return Json.ToString();
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
    
    void setup(FJsonObject json_in)
    {
        PageNumber = json_in.GetIntegerField(TEXT("page"));
        
        json_in.TryGetStringField(TEXT("column"), Column);
        json_in.TryGetStringField(TEXT("before"), Before);
        json_in.TryGetStringField(TEXT("after"), After);

        if(json_in.HasField(TEXT("sort")))
        {
            for (TSharedPtr<FJsonValue> Value : json_in.GetArrayField(TEXT("sort")))
            {
                FSeqMarketplaceSortBy SortBy;
                SortBy.setup(*Value->AsObject().Get());
                Sort.Add(SortBy);
            }
        }

        if(json_in.HasField(TEXT("pageSize")))
            PageSize = json_in.GetIntegerField(TEXT("pageSize"));
        
        More = json_in.GetBoolField(TEXT("more"));
    }

};