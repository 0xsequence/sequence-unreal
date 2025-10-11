#pragma once
#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "EcosystemWallet/Primitives/Config/SeqConfig.h"
#include "Types/BigInt.h"

class SEQUENCEPLUGIN_API FEnvelope
{
public:
	explicit FEnvelope(const FBigInt& InChainId, const FString& InWallet, const TSharedPtr<FSeqConfig>& InConfig, const TSharedPtr<FCalls>& InPayload)
		: ChainId(InChainId), Wallet(InWallet), Config(InConfig), Payload(InPayload) { }
	
	FBigInt ChainId;
	FString Wallet;
	TSharedPtr<FSeqConfig> Config;
	TSharedPtr<FCalls> Payload;
};
