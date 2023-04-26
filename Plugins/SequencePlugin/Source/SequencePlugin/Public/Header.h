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
	FBinaryData Difficulty;
	FBinaryData Number;
	uint64 GasLimit;
	uint64 GasUsed;
	uint64 Time;
	FBinaryData ExtraData;
	Hash256 MixDigest;
	BlockNonce Nonce;
	FBinaryData BaseFee;
};

FHeader JsonToHeader(TSharedPtr<FJsonObject> Json);
