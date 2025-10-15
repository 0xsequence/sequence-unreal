#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceCoder.h"
#include "Util/ValueUtils.h"

TArray<uint8> FCall::Encode()
{
	const uint8 Flag = GetFlag();
	return FByteArrayUtils::ConcatBytes({
		FByteArrayUtils::ByteArrayFromNumber(Flag, 1),
		GetAddressBytes(Flag),
		GetValueBytes(Flag),
		GetDataBytes(Flag),
		GetGasLimitBytes(Flag)});
}

TArray<uint8> FCall::Hash() const
{
	const TArray<uint8> TypeHash = FSequenceCoder::KeccakHash(FByteArrayUtils::StringToBytes(CallTypeHash));
	const TArray<uint8> ToHash = FSequenceCoder::EncodeAddress(To);
	const TArray<uint8> ValueHash = FSequenceCoder::EncodeSignedInt(Value.Value);
	const TArray<uint8> DataHash = FSequenceCoder::KeccakHash(FByteArrayUtils::HexStringToBytes(Data));
	const TArray<uint8> GasLimitHash = FSequenceCoder::EncodeSignedInt(GasLimit.Value);
	const TArray<uint8> DelegateCallHash = FSequenceCoder::EncodeBoolean(DelegateCall);
	const TArray<uint8> OnlyFallbackHash = FSequenceCoder::EncodeBoolean(OnlyFallback);
	const TArray<uint8> BehaviorOnErrorHash = FSequenceCoder::EncodeSignedInt(BehaviorOnError);

	TArray<uint8> Encoded;
	Encoded.Append(TypeHash);
	Encoded.Append(ToHash);
	Encoded.Append(ValueHash);
	Encoded.Append(DataHash);
	Encoded.Append(GasLimitHash);
	Encoded.Append(DelegateCallHash);
	Encoded.Append(OnlyFallbackHash);
	Encoded.Append(BehaviorOnErrorHash);

	UE_LOG(LogTemp, Display, TEXT("call data to hash %s"), *FByteArrayUtils::BytesToHexString(Encoded))

	return FSequenceCoder::KeccakHash(Encoded);
}

uint8 FCall::GetFlag()
{
	int32 Flags = 0;
	TOptional<FString> SelfAddress;

	if (SelfAddress.IsSet() && SelfAddress.GetValue().Equals(To))
	{
		Flags |= 0x01;
	}

	if (Value.Value != "0")
	{
		Flags |= 0x02;
	}

	if (FByteArrayUtils::HexStringToBytes(Data).Num() > 0)
	{
		Flags |= 0x04;
	}

	if (GasLimit.Value != "0")
	{
		Flags |= 0x08;
	}

	if (DelegateCall)
	{
		Flags |= 0x10;
	}

	if (OnlyFallback)
	{
		Flags |= 0x20;
	}

	Flags |= FValueUtils::StringToInt32(BehaviorOnError) << 6;

	return Flags;
}

TArray<uint8> FCall::GetAddressBytes(const uint8& Flag)
{
	if ((Flag & 0x01) == 0 )
	{
		TArray<uint8> ToAddressBytes = FByteArrayUtils::HexStringToBytes(To);

		if (ToAddressBytes.Num() != 20)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid 'to' address %s — must be 20 bytes but got %d"), *To, ToAddressBytes.Num());
		}

		return ToAddressBytes;
	}

	return TArray<uint8>();
}

TArray<uint8> FCall::GetValueBytes(const uint8& Flag)
{
	if ((Flag & 0x02) == 0x02)
	{
		return FByteArrayUtils::PadLeft(Value.Encode(), 32);
	}

	return TArray<uint8>();
}

TArray<uint8> FCall::GetDataBytes(const uint8& Flag)
{
	if ((Flag & 0x04) != 0)
	{
		TArray<uint8> RawData = FByteArrayUtils::HexStringToBytes(Data);
		if (RawData.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("If (flags & 0x04) != 0, then data cannot be empty"));
		}

		const int32 CallDataLength = RawData.Num();

		if (CallDataLength > 0xFFFFFF)
		{
			UE_LOG(LogTemp, Error, TEXT("Data too large: length cannot exceed 0xFFFFFF (16,777,215), given %d"), CallDataLength);
		}

		TArray<uint8> CallDataLengthBytes = FByteArrayUtils::ByteArrayFromNumber(CallDataLength, 3);

		return FByteArrayUtils::ConcatBytes({CallDataLengthBytes, RawData});
	}

	return TArray<uint8>();
}

TArray<uint8> FCall::GetGasLimitBytes(const uint8& Flag)
{
	if ((Flag & 0x08) != 0)
	{
		return FByteArrayUtils::PadLeft(GasLimit.Encode(), 32);
	}

	return TArray<uint8>();
}
