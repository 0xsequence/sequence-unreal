// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Indexer/Indexer_Enums.h"
#include "Dom/JsonObject.h"
#include "SortBy.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSortBy
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString column = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        TEnumAsByte<ESortOrder_Sequence> order = ESortOrder_Sequence::ASC;

    /*
    * Used to get the jsonObjectString of this struct for args and testing
    */
    FString GetJsonString()
    {
        FString ret = "{";
        ret.Append("\"column\":\"");
        ret.Append(column);
        ret.Append("\",\"order\":\"");
        ret.Append(UEnum::GetValueAsString(order.GetValue()));
        ret.Append("\"}");
        return ret;
    }

    /*
    * Used to handle UE's edge cases with json Parsing
    */
    void setup(FJsonObject json_in)
    {
        if (!json_in.TryGetField(TEXT("column")))
            column = json_in.GetStringField(TEXT("column"));

        if (!json_in.TryGetField(TEXT("order")))
            order = ESortOrder_Sequence(json_in.GetIntegerField(TEXT("order")));
    }
};
