#pragma once
#include "Types/BigInt.h"
#include "Util/ByteArrayUtils.h"

class SEQUENCEPLUGIN_API FAttestationAuthData
{
public:
	FAttestationAuthData() : RedirectUrl(""), IssuedAt(FBigInt("0")) { }
	
	TArray<uint8> Encode()
	{
		return FByteArrayUtils::ConcatBytes({
			FByteArrayUtils::ByteArrayFromNumber(RedirectUrl.Len(), 3),
			FByteArrayUtils::StringToBytes(RedirectUrl),
			FByteArrayUtils::PadLeft(IssuedAt.Encode(), 8)
		});
	}

	FString RedirectUrl;
	FBigInt IssuedAt;
};
