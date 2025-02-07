#pragma once

#include "CoreMinimal.h"
#include "SeqFeeBreakdown.h"
#include "Marketplace/Marketplace_Enums.h"
#include "Util/SequenceSupport.h"
#include "SeqGetSwapQuoteRequest.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetSwapQuoteRequest
{
    GENERATED_USTRUCT_BODY()

public:
    static inline bool customConstructor = false; // Flag to determine the constructor used
    static inline bool customGetter = false;
    static FString GetArgs() { return ""; }
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString UserAddress;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString BuyCurrencyAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SellCurrencyAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString BuyAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int ChainID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IncludeApprove;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int SlippagePercentage;
};