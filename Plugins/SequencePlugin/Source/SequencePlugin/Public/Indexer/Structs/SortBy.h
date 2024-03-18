#pragma once

#include "CoreMinimal.h"
#include "Indexer/Indexer_Enums.h"
#include "Dom/JsonObject.h"
#include "SortBy.generated.h"

USTRUCT(BlueprintType)
struct FSortBy
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString column = "";
    UPROPERTY()
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
        if (!json_in.TryGetField("column"))
            column = json_in.GetStringField("column");

        if (!json_in.TryGetField("order"))
            order = ESortOrder_Sequence(json_in.GetIntegerField("order"));
    }
};
