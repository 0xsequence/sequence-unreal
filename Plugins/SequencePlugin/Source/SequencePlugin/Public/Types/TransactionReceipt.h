#pragma once
#include "Dom/JsonObject.h"
#include "BinaryData.h"

struct SEQUENCEPLUGIN_API FTransactionReceipt
{
	FHash256 BlockHash;
	FString BlockNumber;
	FHash256 TransactionHash;
	uint64 TransactionIndex;
	FAddress From;
	FAddress To;
	FString CumulativeGasUsed;
	FString GasUsed;
	FAddress ContractAddress;
	FString Status;
};

FTransactionReceipt SEQUENCEPLUGIN_API JsonToTransactionReceipt(TSharedPtr<FJsonObject> Json);