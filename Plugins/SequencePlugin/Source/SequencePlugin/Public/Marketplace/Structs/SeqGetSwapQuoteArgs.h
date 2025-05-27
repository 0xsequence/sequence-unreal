#pragma once

#include "CoreMinimal.h"
#include "SeqGetSwapQuoteParams.h"
#include "SeqGetSwapQuoteArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetSwapQuoteArgs
{
    GENERATED_USTRUCT_BODY()

public:
    static inline bool customConstructor = false; // Flag to determine the constructor used
    static inline bool customGetter = false;
    static FString GetArgs() { return ""; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "params"))
    FSeqGetSwapQuoteParams Params;
};