#pragma once
#include "BinaryData.h"

struct FContractCall
{
	TOptional<FAddress> From;
	FAddress To;
	TOptional<uint64> Gas;
	TOptional<uint64> GasPrice;
	TOptional<uint64> Value;
	TOptional<FString> Data;

	FString GetJson();
};