#pragma once

#include "SeqLifiSwapRoute.h"
#include "SeqGetLifiSwapRoutesResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetLifiSwapRoutesResponse
{
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "routes"))
    TArray<FSeqLifiSwapRoute> Routes;
};
