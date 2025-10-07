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
};
