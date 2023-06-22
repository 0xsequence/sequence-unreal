#pragma once

#include "CoreMinimal.h"
#include "Indexer_Enums.h"
#include "SortBy.generated.h"

USTRUCT(BlueprintType)
struct FSortBy
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString column;
    UPROPERTY()
        TEnumAsByte<ESortOrder> order;

    void setup(FJsonObject json_in)
    {
        if (!json_in.TryGetField("column"))
            column = json_in.GetStringField("column");

        if (!json_in.TryGetField("order"))
            order = ESortOrder(json_in.GetIntegerField("order"));
    }
};
