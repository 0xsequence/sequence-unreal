#pragma once

#include "CoreMinimal.h"
#include "SeqFeeBreakdown.h"
#include "SeqSwapPrice.h"
#include "Marketplace/Marketplace_Enums.h"
#include "Util/SequenceSupport.h"
#include "SeqGetSwapPriceResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetSwapPriceResponse
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSeqSwapPrice SwapPrice;
};