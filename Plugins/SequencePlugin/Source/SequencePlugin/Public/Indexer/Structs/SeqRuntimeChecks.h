
#pragma once

#include "CoreMinimal.h"
#include "SeqRuntimeChecks.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqRuntimeChecks
{
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        bool running = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        FString syncMode = "";
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
        int64 lastBlockNum = -1;
};