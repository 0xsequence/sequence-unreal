#pragma once

#include "CoreMinimal.h"
#include "Marketplace/Marketplace_Enums.h"
#include "Dom/JsonObject.h"
#include "SeqMarketplaceSortBy.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqMarketplaceSortBy
{
    GENERATED_USTRUCT_BODY()

public:
    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sort")
    FString Column;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sort")
    TEnumAsByte<EMarkeplaceSortOrder> Order;

    FString GetJsonString()
    {
        FString ret = "{";
        ret.Append("\"column\":\"");
        ret.Append(Column);
        ret.Append("\",\"order\":\"");
        ret.Append(UEnum::GetValueAsString(Order.GetValue()));
        ret.Append("\"}");
        return ret;
    }

    /*
    * Used to handle UE's edge cases with json Parsing
    */
    void setup(FJsonObject json_in)
    {
        if (!json_in.TryGetField(TEXT("column")))
            Column = json_in.GetStringField(TEXT("column"));

        if (!json_in.TryGetField(TEXT("order")))
            Order = EMarkeplaceSortOrder(json_in.GetIntegerField(TEXT("order")));
    }
};