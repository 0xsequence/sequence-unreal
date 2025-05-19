#pragma once

#include "SeqGetLifiTokensArgs.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqGetLifiTokensArgs
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "chainIds"))
    TArray<int64> ChainIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "contract"))
    FString Contract;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "tokenId"))
    FString TokenId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "symbol"))
    FString Symbol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "name"))
    FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "priceUsd"))
    float PriceUsd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName = "logoUri"))
    FString LogoUri;
};
