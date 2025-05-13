// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Types/Header.h"
#include "Types/BinaryData.h"
#include "Util/HexUtility.h"

FHeader JsonToHeader(TSharedPtr<FJsonObject> Json)
{
	FHash256 ParentHash = FHash256::From(Json->GetStringField(TEXT("parentHash")));
	FHash256 UncleHash =  FHash256::From(Json->GetStringField(TEXT("sha3Uncles")));
	FAddress Coinbase = FAddress::New(); // TODO 
	FHash256 Root = FHash256::From(Json->GetStringField(TEXT("stateRoot")));
	FHash256 TxHash = FHash256::From(Json->GetStringField(TEXT("transactionsRoot")));
	FHash256 ReceiptHash = FHash256::From(Json->GetStringField(TEXT("receiptsRoot")));
	FBloom Bloom = FBloom::From(Json->GetStringField(TEXT("logsBloom")));
	FUnsizedData Difficulty = HexStringToBinary(Json->GetStringField(TEXT("difficulty")));
	FUnsizedData Number = HexStringToBinary(Json->GetStringField(TEXT("number")));
	uint64 GasLimit = HexStringToUint64(Json->GetStringField(TEXT("gasLimit"))).Get(0);
	uint64 GasUsed = HexStringToUint64(Json->GetStringField(TEXT("gasUsed"))).Get(0);
	uint64 Time = HexStringToUint64(Json->GetStringField(TEXT("timestamp"))).Get(0);
	FUnsizedData ExtraData = HexStringToBinary(Json->GetStringField(TEXT("extraData")));
	FHash256 MixDigest = FHash256::From(Json->GetStringField(TEXT("mixHash")));
	FBlockNonce Nonce = FBlockNonce::From(Json->GetStringField(TEXT("nonce")));
	FUnsizedData BaseFee = FUnsizedData::Empty();

	return FHeader{
		ParentHash, UncleHash, Coinbase, Root, TxHash, ReceiptHash, Bloom, Difficulty, Number, GasLimit, GasUsed, Time, ExtraData, MixDigest, Nonce, BaseFee
	};
}
