#pragma once

#include "SeqSwapToken.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSeqSwapToken
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	int64 Chain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Contract;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString TokenId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Symbol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	int64 Decimals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	int64 Price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	float PriceUsd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString LogoUri;
};
