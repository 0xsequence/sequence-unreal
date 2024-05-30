// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Dom/JsonObject.h"
#include "BinaryData.h"

struct SEQUENCEPLUGIN_API FTransactionReceipt
{
	FHash256 BlockHash;
	uint64 BlockNumber;
	FHash256 TransactionHash;
	uint64 TransactionIndex;
	FAddress From;
	FAddress To;
	uint64 CumulativeGasUsed;
	uint64 GasUsed;
	FAddress ContractAddress;
	FString Status;
};

FTransactionReceipt SEQUENCEPLUGIN_API JsonToTransactionReceipt(TSharedPtr<FJsonObject> Json);