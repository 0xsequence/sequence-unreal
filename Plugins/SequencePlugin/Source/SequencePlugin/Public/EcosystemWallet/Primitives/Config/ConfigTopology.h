#pragma once

#include "CoreMinimal.h"
#include "ConfigLeaf.h"
#include "ConfigNode.h"

class SEQUENCEPLUGIN_API FConfigTopology
{
public:
	FConfigTopology()
	{
		this->Node = nullptr;
		this->Leaf = nullptr;
	}

	FConfigTopology(const TSharedPtr<FConfigNode>& InNode)
	{
		this->Node = InNode;
		this->Leaf = nullptr;
	}

	FConfigTopology(const TSharedPtr<FConfigLeaf>& InLeaf)
	{
		this->Node = nullptr;
		this->Leaf = InLeaf;
	}

	bool IsNode() const
	{
		return this->Node != nullptr && this->Node.IsValid();
	}
	
	bool IsLeaf() const
	{
		return this->Leaf != nullptr && this->Leaf.IsValid();
	}

	TSharedPtr<FConfigLeaf> FindSignerLeaf(const FString& Address) const;
	
	static TSharedPtr<FConfigTopology> FromServiceConfigTree(const TSharedPtr<FJsonValue>& Input);

	TSharedPtr<FConfigNode> Node;
	TSharedPtr<FConfigLeaf> Leaf;

	static constexpr uint8 FlagSignatureHash = 0;
	static constexpr uint8 FlagAddress = 1;
	static constexpr uint8 FlagSignatureErc1271 = 2;
	static constexpr uint8 FlagNode = 3;
	static constexpr uint8 FlagBranch = 4;
	static constexpr uint8 FlagSubdigest = 5;
	static constexpr uint8 FlagNested = 6;
	static constexpr uint8 FlagSignatureEthSign = 7;
	static constexpr uint8 FlagSignatureAnyAddressSubdigest = 8;
	static constexpr uint8 FlagSignatureSapient = 9;
	static constexpr uint8 FlagSignatureSapientCompact = 10;
};
