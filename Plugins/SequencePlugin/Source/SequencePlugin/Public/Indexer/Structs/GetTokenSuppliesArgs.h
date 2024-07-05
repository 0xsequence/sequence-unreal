// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "GetTokenSuppliesArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenSuppliesArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
        FString contractAddress = "";
    UPROPERTY(BlueprintReadWrite)
        bool includeMetaData = false;
    UPROPERTY(BlueprintReadWrite)
        FPage page;

    bool customGetter = false;
    FString GetArgs() { return ""; }//no custom getter needed here!
};