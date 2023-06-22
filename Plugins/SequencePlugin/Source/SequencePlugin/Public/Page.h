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

    void setup(FJsonObject json_in)
    {
        /*
        if (json_in.TryGetField("more") != nullptr)
            more = json_in.GetBoolField("more");

        if (json_in.TryGetField("pageSize") != nullptr)
            pageSize = json_in.GetIntegerField("pageSize");

        if (json_in.TryGetField("after") != nullptr)
            after = *json_in.GetStringField("after");

        if (json_in.TryGetField("before") != nullptr)
            after = *json_in.GetStringField("before");

        if (json_in.TryGetField("column") != nullptr)
            column = json_in.GetStringField("column");

        if (json_in.TryGetField("page") != nullptr)
            page = json_in.GetIntegerField("page");

        if (json_in.TryGetField("sort") != nullptr)
        {
            TArray<TSharedPtr<FJsonValue>> list = json_in.GetArrayField("sort");

            for (int32 i = 0; i < list.Num(); i++)
            {
                sort.Add(FSortBy());//create new struct!
                sort[i].setup(*list[i].Get()->AsObject().Get());//do manual setup!
            }

        }
        */

    }
};