#pragma once
#include "EcosystemWallet/Primitives/Config/ConfigTopology.h"

class SEQUENCEPLUGIN_API FSignatureUtils
{
public:
	static TTuple<TArray<TSharedPtr<FConfigTopology>>, TArray<uint8>> ParseBranch(const TArray<uint8>& Signature);
	static TSharedPtr<FConfigTopology> FoldNodes(const TArray<TSharedPtr<FConfigTopology>>& Nodes);
};
