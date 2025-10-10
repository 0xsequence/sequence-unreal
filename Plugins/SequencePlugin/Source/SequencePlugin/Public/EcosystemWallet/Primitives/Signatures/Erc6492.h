#pragma once
#include "Erc6492DecodingResult.h"

class SEQUENCEPLUGIN_API FErc6492
{
public:
	FErc6492() : Address(TEXT("")), Data(TArray<uint8>()) { }
	explicit FErc6492(const FString& Address, const TArray<uint8>& Data) : Address(Address), Data(Data) { }

	static FErc6492DecodingResult Decode(const TArray<uint8>& Signature);
	
	FString Address;
	TArray<uint8> Data;
};
