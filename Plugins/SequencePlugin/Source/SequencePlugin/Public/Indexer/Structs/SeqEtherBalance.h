// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "SeqEtherBalance.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqEtherBalance
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString accountAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        int64 balanceWei = -1;
};