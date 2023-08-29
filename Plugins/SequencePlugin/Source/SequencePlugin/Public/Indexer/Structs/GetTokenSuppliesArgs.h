#pragma once

#include "CoreMinimal.h"
#include "Page.h"
#include "GetTokenSuppliesArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetTokenSuppliesArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString contractAddress = "";
    UPROPERTY()
        bool includeMetaData = false;
    UPROPERTY()
        FPage page;

    bool customGetter = false;
    FString GetArgs() { return ""; }//no custom getter needed here!
};