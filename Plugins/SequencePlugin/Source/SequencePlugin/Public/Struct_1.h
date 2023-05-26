#pragma once

#include "CoreMinimal.h"
#include "Struct_1.generated.h"

USTRUCT(BlueprintType)
struct FStruct_1
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()//only visible in C++
        int32 age = 1;
    UPROPERTY()
        FString name = "struct_1";
};