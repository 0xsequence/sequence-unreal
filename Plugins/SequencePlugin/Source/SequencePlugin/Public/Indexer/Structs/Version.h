// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Version.generated.h"

USTRUCT(BlueprintType)
struct FVersion
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString webrpcVersion = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString schemaVersion = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString schemaHash = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString appVersion = "";
};