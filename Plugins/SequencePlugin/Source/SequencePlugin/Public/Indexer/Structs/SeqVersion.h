// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqVersion.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqVersion
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString webrpcVersion = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString schemaVersion = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString schemaHash = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString appVersion = "";
};