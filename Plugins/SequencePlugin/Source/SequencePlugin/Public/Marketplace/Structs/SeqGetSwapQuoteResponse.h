#pragma once

#include "CoreMinimal.h"
#include "SeqSwapQuote.h"
#include "SeqGetSwapQuoteResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetSwapQuoteResponse
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "quote"))
    FSeqSwapQuote Quote;
};