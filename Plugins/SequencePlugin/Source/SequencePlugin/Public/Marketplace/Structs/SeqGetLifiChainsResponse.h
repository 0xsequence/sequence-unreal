#pragma once

#include "SeqGetLifiChainsResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetLifiChainsResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "chainIds"))
    TArray<int64> ChainIds;
};
