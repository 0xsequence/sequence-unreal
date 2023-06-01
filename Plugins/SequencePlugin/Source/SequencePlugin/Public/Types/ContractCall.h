#pragma once
#include "BinaryData.h"

struct ContractCall
{
	TOptional<FAddress> From;
	FAddress To;
	TOptional<uint64> Gas;
	TOptional<uint64> GasPrice;
	TOptional<uint64> Value;
	TOptional<FHash256> Data;

	FString GetJson();
};


