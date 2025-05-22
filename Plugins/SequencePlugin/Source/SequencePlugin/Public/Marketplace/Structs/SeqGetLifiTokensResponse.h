#pragma once

#include "SeqLifiToken.h"
#include "SeqGetLifiTokensResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetLifiTokensResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "tokens"))
    TArray<FSeqLifiToken> Tokens;
};
