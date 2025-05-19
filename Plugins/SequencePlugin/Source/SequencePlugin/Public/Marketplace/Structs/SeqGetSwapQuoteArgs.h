#pragma once

#include "CoreMinimal.h"
#include "SeqFeeBreakdown.h"
#include "Marketplace/Marketplace_Enums.h"
#include "Util/SequenceSupport.h"
#include "SeqGetSwapQuoteArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetSwapQuoteArgs
{
    GENERATED_USTRUCT_BODY()

public:
    static inline bool customConstructor = false; // Flag to determine the constructor used
    static inline bool customGetter = false;
    static FString GetArgs() { return ""; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    int64 ChainId;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString WalletAddress;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString FromTokenAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString ToTokenAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString FromTokenAmount;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    FString ToTokenAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    bool IncludeApprove;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    float SlippageBps;
};