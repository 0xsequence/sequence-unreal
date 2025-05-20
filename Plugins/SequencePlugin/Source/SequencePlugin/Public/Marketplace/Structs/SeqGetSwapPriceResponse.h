#pragma once

#include "CoreMinimal.h"
#include "SeqSwapPrice.h"
#include "SeqGetSwapPriceResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetSwapPriceResponse
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FSeqSwapPrice SwapPrice;
};