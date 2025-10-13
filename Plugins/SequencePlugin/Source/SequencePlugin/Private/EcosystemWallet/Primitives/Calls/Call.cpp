#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceCoder.h"

TArray<uint8> FCall::Hash() const
{
	TArray<uint8> TypeHash = TArray<uint8>(); // FStaticBytesCoder::Encode(FString(CallTypeHash));
	TArray<uint8> ToHash = TArray<uint8>(); // FAddressCoder::Encode(To);
	TArray<uint8> ValueHash = TArray<uint8>(); // FEthAbiBridge::EncodeBigInteger(std::to_string(Value));
	TArray<uint8> DataHash = FSequenceCoder::KeccakHash(FByteArrayUtils::HexStringToBytes(Data));
	TArray<uint8> GasLimitHash = TArray<uint8>(); // FEthAbiBridge::EncodeBigInteger(FString::Printf(TEXT("%i"), GasLimit));
	TArray<uint8> DelegateCallHash = TArray<uint8>(); // FBooleanCoder::Encode(bDelegateCall);
	TArray<uint8> OnlyFallbackHash = TArray<uint8>(); // FBooleanCoder::Encode(bOnlyFallback);
	TArray<uint8> BehaviorOnErrorHash = TArray<uint8>(); // FNumberCoder::Encode(BehaviorOnError);

	TArray<uint8> Encoded;
	Encoded.Append(TypeHash);
	Encoded.Append(ToHash);
	Encoded.Append(ValueHash);
	Encoded.Append(DataHash);
	Encoded.Append(GasLimitHash);
	Encoded.Append(DelegateCallHash);
	Encoded.Append(OnlyFallbackHash);
	Encoded.Append(BehaviorOnErrorHash);

	return FSequenceCoder::KeccakHash(Encoded);
}

