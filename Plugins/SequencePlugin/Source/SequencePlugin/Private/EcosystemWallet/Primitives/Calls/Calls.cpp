#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "Util/ByteArrayUtils.h"

TArray<uint8> FCalls::Encode()
{
	TArray<uint8> FlagBytes = FByteArrayUtils::ByteArrayFromNumber(GetFlag(), 1);
	TArray<uint8> SpaceBytes = GetSpaceBytes();
	TArray<uint8> NonceBytes = GetNonceBytes();
	TArray<uint8> CallsCountBytes = GetCallsCount();
	TArray<uint8> CallsBytes = GetCallsBytes();

	return FByteArrayUtils::ConcatBytes({FlagBytes, SpaceBytes, NonceBytes, CallsCountBytes, CallsBytes});
}

uint8 FCalls::GetFlag()
{
	uint8 GlobalFlag = 0;

	if (Space.Value == "0")
	{
		GlobalFlag |= 0x01;
	}

	constexpr uint8 NonceBytesNeeded = 1;
	GlobalFlag |= static_cast<uint8>(NonceBytesNeeded << 1);

	if (Calls.Num() == 1)
	{
		GlobalFlag |= 0x10;
	}

	if (GetCallsCountSize() > 1)
	{
		GlobalFlag |= 0x20;
	}

	return GlobalFlag;
}

uint8 FCalls::GetCallsCountSize()
{
	uint8 Size = 0;
	const uint8 CallsLen = Calls.Num();
	
	if (CallsLen != 0)
	{
		if (CallsLen < 256)
		{
			Size = 1;
		}
		else if (CallsLen < 65536)
		{
			Size = 2;
		}
	}

	return Size;
}

TArray<uint8> FCalls::GetSpaceBytes()
{
	if (Space.Value == "0")
	{
		return TArray<uint8>();
	}

	return Space.Encode();
}

TArray<uint8> FCalls::GetNonceBytes()
{
	if (Nonce.Value == "0")
	{
		return TArray<uint8>();
	}

	return Nonce.Encode();
}

TArray<uint8> FCalls::GetCallsCount()
{
	const uint8 CountSize = GetCallsCountSize();
	if (Calls.Num() != 1)
	{
		if (CountSize > 2 || CountSize <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Calls count is invalid"));
		}

		return FByteArrayUtils::ByteArrayFromNumber(CountSize, 1);
	}

	return TArray<uint8>();
}

TArray<uint8> FCalls::GetCallsBytes()
{
	TArray<TArray<uint8>> CallsBytes;
	for (FCall Call : Calls)
	{
		CallsBytes.Add(Call.Encode());
	}
	
	return FByteArrayUtils::ConcatBytes(CallsBytes);
}
