#pragma once

#include "Dom/JsonObject.h"
#include "SaleDetails.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FSaleDetails
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonFieldName = "cost"), Category = "0xSequence")
	int64 Cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonFieldName = "supplyCap"), Category = "0xSequence")
	int64 SupplyCap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonFieldName = "startTime"), Category = "0xSequence")
	int64 StartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonFieldName = "endTime"), Category = "0xSequence")
	int64 EndTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonFieldName = "merkleRoot"), Category = "0xSequence")
	FString MerkleRoot;
};
