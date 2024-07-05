// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Version.generated.h"

USTRUCT(BlueprintType)
struct FVersion
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
        FString webrpcVersion = "";
    UPROPERTY(BlueprintReadWrite)
        FString schemaVersion = "";
    UPROPERTY(BlueprintReadWrite)
        FString schemaHash = "";
    UPROPERTY(BlueprintReadWrite)
        FString appVersion = "";
};