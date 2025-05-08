#pragma once

#include "CoreMinimal.h"
#include "SeqFeeBreakdown.h"
#include "Marketplace/Marketplace_Enums.h"
#include "Util/SequenceSupport.h"
#include "SeqGetSwapPricesArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetSwapPricesArgs
{
    GENERATED_USTRUCT_BODY()

public:
    static inline bool customConstructor = false; // Flag to determine the constructor used
    static inline bool customGetter = false;
    static FString GetArgs() { return ""; }
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString UserAddress;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString BuyCurrencyAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString BuyAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    int ChainID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    int SlippagePercentage;
};