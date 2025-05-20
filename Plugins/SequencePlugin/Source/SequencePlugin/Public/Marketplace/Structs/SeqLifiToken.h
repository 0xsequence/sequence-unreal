#pragma once

#include "SeqLifiToken.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqLifiToken
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName="chainId"))
	int64 ChainId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName="contractAddress"))
	FString ContractAddress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence", meta = (JsonFieldName="tokenId"))
	FString TokenId;
};
