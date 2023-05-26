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
};
