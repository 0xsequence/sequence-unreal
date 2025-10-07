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

	FConfigTopology(FConfigNode InNode)
	{
		this->Node = &InNode;
		this->Leaf = nullptr;
	}

	FConfigTopology(FConfigLeaf InLeaf)
	{
		this->Node = nullptr;
		this->Leaf = &InLeaf;
	}

	bool IsNode() const
	{
		return Node != nullptr;
	}
	
	bool IsLeaf() const
	{
		return this->Leaf != nullptr;
	}

	FConfigLeaf* FindSignerLeaf(const FString& Address) const;
	
	static TSharedPtr<FConfigTopology> FromServiceConfigTree(const FString& Input);

	FConfigNode* Node;
	FConfigLeaf* Leaf;
};
