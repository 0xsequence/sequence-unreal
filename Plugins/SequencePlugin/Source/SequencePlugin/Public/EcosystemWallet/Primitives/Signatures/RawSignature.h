#pragma once
#include "Erc6492.h"
#include "EcosystemWallet/Primitives/Config/SeqConfig.h"

class SEQUENCEPLUGIN_API FRawSignature
{
public:
	TArray<uint8> Encode(bool SkipCheckpointerData = false, bool SkipCheckpointerAddress = false);

	static TSharedPtr<FRawSignature> Decode(const TArray<uint8>& Erc6492Signature);

	bool NoChainId;
	TArray<uint8> CheckpointerData;
	USeqConfig* Configuration;
	TArray<FRawSignature> Suffix;
	TSharedPtr<FErc6492> Erc6492;
};
