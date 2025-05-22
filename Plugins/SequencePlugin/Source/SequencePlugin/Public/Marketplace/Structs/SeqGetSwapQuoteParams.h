#pragma once

#include "CoreMinimal.h"
#include "SeqGetSwapQuoteParams.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetSwapQuoteParams
{
    GENERATED_USTRUCT_BODY()

public:
    static inline bool customConstructor = false; // Flag to determine the constructor used
    static inline bool customGetter = false;
    static FString GetArgs() { return ""; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "chainId"))
    int64 ChainId;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "walletAddress"))
    FString WalletAddress;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "fromTokenAddress"))
    FString FromTokenAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "toTokenAddress"))
    FString ToTokenAddress;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "fromTokenAmount"))
    FString FromTokenAmount;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "toTokenAmount"))
    FString ToTokenAmount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "includeApprove"))
    bool IncludeApprove;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "slippageBps"))
    float SlippageBps;
};