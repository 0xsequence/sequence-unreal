#pragma once
#include "BinaryData.h"
#include "Types.h"

struct FHeader
{
	FHash256 ParentHash;
	FHash256 UncleHash;
	FAddress Coinbase;
	FHash256 Root;
	FHash256 TxHash;
	FHash256 ReceiptHash;
	FBloom Bloom;
	FNonUniformData Difficulty;
	FNonUniformData Number;
	uint64 GasLimit;
	uint64 GasUsed;
	uint64 Time;
	FNonUniformData ExtraData;
	FHash256 MixDigest;
	FBlockNonce Nonce;
	FNonUniformData BaseFee;
};

FHeader JsonToHeader(TSharedPtr<FJsonObject> Json);
