#pragma once

#include "CoreMinimal.h"
#include "SeqGetChainIDArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetChainIDArgs
{
    GENERATED_USTRUCT_BODY()
public:
    //this guy is a nothing burger?

    bool customGetter = false;
    FString GetArgs() { return ""; }//no custom getter needed
};