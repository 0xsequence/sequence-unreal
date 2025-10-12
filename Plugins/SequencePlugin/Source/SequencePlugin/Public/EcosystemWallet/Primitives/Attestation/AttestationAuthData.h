#pragma once
#include "Types/BigInt.h"

class SEQUENCEPLUGIN_API FAttestationAuthData
{
public:
	FAttestationAuthData() : RedirectUrl(""), IssuedAt(FBigInt("0")) { }
	
	TArray<uint8> Encode()
	{
		return TArray<uint8>();
	}

	FString RedirectUrl;
	FBigInt IssuedAt;
};
