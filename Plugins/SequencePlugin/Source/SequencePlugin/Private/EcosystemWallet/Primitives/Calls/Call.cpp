#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceCoder.h"

TArray<uint8> FCall::Hash() const
{
	const TArray<uint8> TypeHash = FSequenceCoder::KeccakHash(FByteArrayUtils::StringToBytes(CallTypeHash));
	const TArray<uint8> ToHash = FSequenceCoder::EncodeAddress(To);
	const TArray<uint8> ValueHash = FSequenceCoder::EncodeSignedInt(Value.Value);
	const TArray<uint8> DataHash = FSequenceCoder::KeccakHash(FByteArrayUtils::HexStringToBytes(Data));
	const TArray<uint8> GasLimitHash = FSequenceCoder::EncodeSignedInt(GasLimit.Value);
	const TArray<uint8> DelegateCallHash = FSequenceCoder::EncodeBoolean(DelegateCall);
	const TArray<uint8> OnlyFallbackHash = FSequenceCoder::EncodeBoolean(OnlyFallback);
	const TArray<uint8> BehaviorOnErrorHash = FSequenceCoder::EncodeSignedInt(BehaviourOnError);

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

