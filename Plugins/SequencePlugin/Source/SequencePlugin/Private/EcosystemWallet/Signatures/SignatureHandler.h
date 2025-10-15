#pragma once
#include "EcosystemWallet/Primitives/Config/ConfigTopology.h"
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"
#include "EcosystemWallet/Primitives/Signatures/RawSignature.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfLeaf.h"

class SEQUENCEPLUGIN_API FSignatureHandler
{
public:
	static TSharedPtr<FRawSignature> EncodeSignature(const TSharedPtr<FEnvelope>& Envelope, const FString& SessionsImageHash);

private:
	static TSharedPtr<FConfigTopology> FillLeaves(
		const TSharedPtr<FEnvelope>& Envelope,
		const TSharedPtr<FConfigTopology>& ConfigTopology,
		const FString& SessionsImageHash);
	
	static TSharedPtr<FSignatureOfLeaf> SignatureForLeaf(
		const TSharedPtr<FEnvelope>& Envelope,
		const TSharedPtr<FConfigLeaf>& Leaf,
		const FString& SessionsImageHash);
};
