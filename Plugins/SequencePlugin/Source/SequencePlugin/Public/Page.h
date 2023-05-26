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
        int32 page;
    UPROPERTY()
        FString column;
    UPROPERTY()
        UObject before;
    UPROPERTY()
        UObject after;//not sure what these are for yet!
    UPROPERTY()
        TArray<FSortBy> sort;
    UPROPERTY()
        int32 pageSize = 50;
    UPROPERTY()
        bool more;
};