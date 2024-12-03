#pragma once

#include "CoreMinimal.h"
#include "SeqCurrency.h"
#include "SeqListCurrenciesResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqListCurrenciesResponse
{
    GENERATED_USTRUCT_BODY()

public:
    // Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Page")
    TArray<FSeqCurrency> Currencies;
};