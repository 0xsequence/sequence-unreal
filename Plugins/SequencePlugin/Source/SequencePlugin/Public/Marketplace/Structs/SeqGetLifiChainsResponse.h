#pragma once

#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "SeqGetLifiChainsResponse.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetLifiChainsResponse
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName="chains"))
    TArray<int64> Chains;
};
