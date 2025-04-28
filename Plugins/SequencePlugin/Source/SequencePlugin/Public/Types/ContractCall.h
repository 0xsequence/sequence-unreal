// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "BinaryData.h"
#include "ContractCall.generated.h"

USTRUCT(BlueprintType)
struct SEQUENCEPLUGIN_API FContractCall
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonFieldName = "from"), Category = "0xSequence")
	FString From;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonFieldName = "to"), Category = "0xSequence")
	FString To;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonFieldName = "gas"), Category = "0xSequence")
	int64 Gas = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonFieldName = "gasPrice"), Category = "0xSequence")
	int64 GasPrice = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonFieldName = "value"), Category = "0xSequence")
	int64 Value = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (JsonFieldName = "data"), Category = "0xSequence")
	FString Data;

	FString GetJson();
};