#pragma once
#include "EcosystemWallet/Primitives/Config/ConfigTopology.h"
#include "EcosystemWallet/Primitives/Signatures/RawSignature.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfLeaf.h"

class SEQUENCEPLUGIN_API SignatureHandler
{
public:
	static TSharedPtr<FRawSignature> EncodeSignature();

private:
	static TSharedPtr<FConfigTopology> FillLeaves();
	static TSharedPtr<FSignatureOfLeaf> SignatureForLeaf(const TSharedPtr<FConfigLeaf>& Leaf);
};
