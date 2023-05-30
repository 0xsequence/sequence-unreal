#pragma once

#include "CoreMinimal.h"
#include "Struct_1.h"
#include "Struct_0.generated.h"

USTRUCT(BlueprintType)
struct FStruct_0
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        TMap<FString,FStruct_1> data_map;
};