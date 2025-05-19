#pragma once

#include "SeqGetLifiTokensArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetLifiTokensArgs
{
	GENERATED_USTRUCT_BODY()

public:
	static inline bool customGetter = false;
	static FString GetArgs() { return ""; }
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "chainIds"))
    TArray<int64> ChainIds;
};
