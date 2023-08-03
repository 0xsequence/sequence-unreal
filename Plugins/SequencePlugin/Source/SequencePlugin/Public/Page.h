#pragma once

#include "CoreMinimal.h"
#include "SortBy.h"
#include "Page.generated.h"

USTRUCT(BlueprintType)
struct FPage
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
    int32 page = 0;
    UPROPERTY()
    FString column;
    UPROPERTY()
    FString before;//based on observations from what I'm getting these are strings!
    UPROPERTY()
    FString after;
    UPROPERTY()
    TArray<FSortBy> sort;
    UPROPERTY()
    int32 pageSize = 50;
    UPROPERTY()
    bool more;

    TSharedPtr<FJsonObject> Get()
    {
        TSharedPtr<FJsonObject> ret = MakeShareable<FJsonObject>(new FJsonObject);
        ret.Get()->SetNumberField("page", page);
        ret.Get()->SetStringField("column", column);
        ret.Get()->SetStringField("before", before);
        ret.Get()->SetStringField("after", after);

        FString sortString = "[";
        for (FSortBy s : sort)
        {
            sortString.Append(s.Get());
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

    void setup(FJsonObject json_in)
    {}
};