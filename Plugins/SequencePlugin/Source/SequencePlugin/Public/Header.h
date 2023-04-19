#pragma once
#include "Types.h"

struct FHeader
{
	Hash256 ParentHash;
	Hash256 UncleHash;
	Address Coinbase;
	Hash256 Root;
	Hash256 TxHash;
	Hash256 ReceiptHash;
	Bloom Bloom;
	FBigNum Difficulty;
	FBigNum Number;
	uint64 GasLimit;
	uint64 GasUsed;
	uint64 Time;
	FBigNum ExtraData;
	Hash256 MixDigest;
	BlockNonce Nonce;
	FBigNum BaseFee;
};

FHeader JsonToHeader(TSharedPtr<FJsonObject> Json);
