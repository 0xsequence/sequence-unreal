#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "SeqSwapPrice.h"
#include "SeqGetSwapPricesResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FGetSwapPricesResponse
{
    GENERATED_USTRUCT_BODY()
    

public:
    static inline bool customConstructor = false; // Flag to determine the constructor used
    void construct(FJsonObject obj) {}
    void Setup(FJsonObject obj) {}
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
    TArray<FSeqSwapPrice> SwapPrices;

};