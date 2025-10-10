#include "EcosystemWallet/Primitives/Signatures/Erc6492.h"

FErc6492DecodingResult FErc6492::Decode(const TArray<uint8>& Signature)
{
	FErc6492DecodingResult Result {
		TArray<uint8>(),
		MakeShared<FErc6492>(FErc6492())
	};
	
	return Result;
}
