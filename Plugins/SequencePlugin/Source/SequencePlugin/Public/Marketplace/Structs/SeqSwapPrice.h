#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Util/SequenceSupport.h"
#include "SeqSwapPrice.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqSwapPrice
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString CurrencyAddress;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    int64 Price;
};