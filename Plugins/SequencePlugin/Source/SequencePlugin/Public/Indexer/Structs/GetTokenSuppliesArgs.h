// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "GetTokenSuppliesArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetTokenSuppliesArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FString contractAddress = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        bool includeMetaData = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
        FPage page;

    bool customGetter = false;
    FString GetArgs() { return ""; }//no custom getter needed here!
};