#pragma once
#include "BinaryData.h"

struct FTransactionReceipt
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
	// logs?
	FString Status;
};

FTransactionReceipt JsonToTransactionReceipt(TSharedPtr<FJsonObject> Json);