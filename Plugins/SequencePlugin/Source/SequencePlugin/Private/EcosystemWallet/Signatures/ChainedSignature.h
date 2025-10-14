#pragma once
#include "EcosystemWallet/Primitives/Signatures/RawSignature.h"

class SEQUENCEPLUGIN_API FChainedSignature
{
public:
	explicit FChainedSignature(const TArray<TSharedPtr<FRawSignature>>& Signatures) : Signatures(Signatures) { }
	
	TArray<uint8> Encode();
	
	TArray<TSharedPtr<FRawSignature>> Signatures;
};
