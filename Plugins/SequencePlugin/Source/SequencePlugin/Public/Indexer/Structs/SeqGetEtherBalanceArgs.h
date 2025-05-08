
#pragma once

#include "CoreMinimal.h"
#include "SeqGetEtherBalanceArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetEtherBalanceArgs
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
        FString accountAddress = "";

    bool customGetter = false;
    FString GetArgs() { return ""; }//no custom getter needed here!
};