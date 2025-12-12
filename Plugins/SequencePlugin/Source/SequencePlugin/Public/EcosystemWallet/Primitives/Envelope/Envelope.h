#pragma once
#include "EcosystemWallet/Primitives/Calls/Calls.h"
#include "EcosystemWallet/Primitives/Config/SeqConfig.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfLeaf.h"
#include "Types/BigInt.h"

class SEQUENCEPLUGIN_API FEnvelope
{
public:
	explicit FEnvelope(const FBigInt& InChainId, const FString& InWallet, const TSharedPtr<FSeqConfig>& InConfig, const TSharedPtr<FCalls>& InPayload)
		: ChainId(InChainId), Wallet(InWallet), Config(InConfig), Payload(InPayload) { }
	
	void Sign(const TSharedPtr<FSignatureOfLeaf>& Signature)
	{
		Signatures = TArray<TSharedPtr<FSignatureOfLeaf>>();
		Signatures.Add(Signature);
	}
	
	FBigInt ChainId;
	FString Wallet;
	TSharedPtr<FSeqConfig> Config;
	TSharedPtr<FCalls> Payload;
	TArray<TSharedPtr<FSignatureOfLeaf>> Signatures;
};
