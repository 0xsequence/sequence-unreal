#include "Util/SequenceCoder.h"
#include "Eth/Crypto.h"
#include "EthAbi/EthAbiBridge.h"
#include "Util/ByteArrayUtils.h"

TArray<uint8> FSequenceCoder::KeccakHash(const TArray<uint8>& Data)
{
	FUnsizedData UnsizedData = FUnsizedData(Data);
	return *GetKeccakHash(UnsizedData).Arr.Get();
}

TArray<uint8> FSequenceCoder::EncodeAddress(const FString& Address)
{
	FString CurrentAddress = Address;
	if (CurrentAddress.StartsWith(TEXT("0x")))
	{
		CurrentAddress = CurrentAddress.RightChop(2);
	}

	const int32 PadCount = 64 - CurrentAddress.Len();
	const FString EncodedString = FString::ChrN(PadCount, TEXT('0')) + CurrentAddress;

	return FByteArrayUtils::HexStringToBytes(EncodedString);
}

TArray<uint8> FSequenceCoder::EncodeSignedInt(const FString& Value)
{
	return FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(Value), 32);
}

TArray<uint8> FSequenceCoder::EncodeBoolean(const bool& Value)
{
	const FString Hex = FString::ChrN(63, TEXT('0')) + (Value ? TEXT("1") : TEXT("0"));
	return FByteArrayUtils::HexStringToBytes(Hex);
}
