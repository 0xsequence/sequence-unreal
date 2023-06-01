#include "Types/Header.h"
#include "Types/BinaryData.h"
#include "HexUtility.h"

FHeader JsonToHeader(TSharedPtr<FJsonObject> Json)
{
	FHash256 ParentHash = FHash256::From(Json->GetStringField("parentHash"));
	FHash256 UncleHash =  FHash256::From(Json->GetStringField("sha3Uncles"));
	FAddress Coinbase = FAddress::New(); // TODO 
	FHash256 Root = FHash256::From(Json->GetStringField("stateRoot"));
	FHash256 TxHash = FHash256::From(Json->GetStringField("transactionsRoot"));
	FHash256 ReceiptHash = FHash256::From(Json->GetStringField("receiptsRoot"));
	FBloom Bloom = FBloom::From(Json->GetStringField("logsBloom"));
	FNonUniformData Difficulty = HexStringToBinary(Json->GetStringField("difficulty"));
	FNonUniformData Number = HexStringToBinary(Json->GetStringField("number"));
	uint64 GasLimit = HexStringToUint64(Json->GetStringField("gasLimit")).Get(0);
	uint64 GasUsed = HexStringToUint64(Json->GetStringField("gasUsed")).Get(0);
	uint64 Time = HexStringToUint64(Json->GetStringField("timestamp")).Get(0);
	FNonUniformData ExtraData = HexStringToBinary(Json->GetStringField("extraData"));
	FHash256 MixDigest = FHash256::From(Json->GetStringField("mixHash"));
	FBlockNonce Nonce = FBlockNonce::From(Json->GetStringField("nonce"));
	FNonUniformData BaseFee = FNonUniformData{nullptr, 0};

	return FHeader{
		ParentHash, UncleHash, Coinbase, Root, TxHash, ReceiptHash, Bloom, Difficulty, Number, GasLimit, GasUsed, Time, ExtraData, MixDigest, Nonce, BaseFee
	};
}
