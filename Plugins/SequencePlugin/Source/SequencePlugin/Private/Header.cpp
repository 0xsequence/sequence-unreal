#include "Header.h"
#include "Types.h"
#include "HexUtility.h"

FHeader JsonToHeader(TSharedPtr<FJsonObject> Json)
{
	Hash256 ParentHash = HexStringToHash256(Json->GetStringField("parentHash"));
	Hash256 UncleHash = HexStringToHash256(Json->GetStringField("sha3Uncles"));
	Address Coinbase = nullptr;
	Hash256 Root = HexStringToHash256(Json->GetStringField("stateRoot"));
	Hash256 TxHash = HexStringToHash256(Json->GetStringField("transactionsRoot"));
	Hash256 ReceiptHash = HexStringToHash256(Json->GetStringField("receiptsRoot"));
	Bloom Bloom = HexStringToHash(GBloomByteLength, Json->GetStringField("logsBloom"));
	FBinaryData Difficulty = HexStringtoBinary(Json->GetStringField("difficulty"));
	FBinaryData Number = HexStringtoBinary(Json->GetStringField("number"));
	uint64 GasLimit = HexStringToInt64(Json->GetStringField("gasLimit")).Get(0);
	uint64 GasUsed = HexStringToInt64(Json->GetStringField("gasUsed")).Get(0);
	uint64 Time = HexStringToInt64(Json->GetStringField("timestamp")).Get(0);
	FBinaryData ExtraData = HexStringtoBinary(Json->GetStringField("extraData"));
	Hash256 MixDigest = HexStringToHash256(Json->GetStringField("mixHash"));
	BlockNonce Nonce = HexStringToHash(GBlockNonceByteLength, Json->GetStringField("nonce"));
	FBinaryData BaseFee = FBinaryData{nullptr, 0};

	return FHeader{
		ParentHash, UncleHash, Coinbase, Root, TxHash, ReceiptHash, Bloom, Difficulty, Number, GasLimit, GasUsed, Time, ExtraData, MixDigest, Nonce, BaseFee
	};
}
