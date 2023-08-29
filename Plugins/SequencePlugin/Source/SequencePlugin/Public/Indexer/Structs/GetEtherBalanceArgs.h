#pragma once

#include "CoreMinimal.h"
#include "GetEtherBalanceArgs.generated.h"

USTRUCT(BlueprintType)
struct FGetEtherBalanceArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString accountAddress = "";

    bool customGetter = false;
    FString GetArgs() { return ""; }//no custom getter needed here!
};