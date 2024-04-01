// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Struct_1.generated.h"

USTRUCT(BlueprintType)
struct FStruct_1
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()//only visible in C++
        TMap<FString,int32> int_data;
};