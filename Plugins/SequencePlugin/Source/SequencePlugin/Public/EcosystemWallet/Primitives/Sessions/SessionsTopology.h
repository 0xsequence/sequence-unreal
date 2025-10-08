#pragma once
#include "SessionsBranch.h"
#include "SessionsLeaf.h"

class SEQUENCEPLUGIN_API FSessionsTopology
{
public:
	FSessionsTopology()
	{
		this->Branch = nullptr;
		this->Leaf = nullptr;
	}

	FSessionsTopology(const TSharedPtr<FSessionsBranch>& InBranch)
	{
		this->Branch = InBranch;
		this->Leaf = nullptr;
	}

	FSessionsTopology(const TSharedPtr<FSessionsLeaf>& InLeaf)
	{
		this->Branch = nullptr;
		this->Leaf = InLeaf;
	}

	bool IsBranch() const
	{
		return this->Branch != nullptr && this->Branch.IsValid();
	}
	
	bool IsLeaf() const
	{
		return this->Leaf != nullptr && this->Leaf.IsValid();
	}

	TSharedPtr<FSessionsLeaf> FindIdentitySigner(const FString& Address) const;
	
	static TSharedPtr<FSessionsTopology> FromServiceConfigTree(const TSharedPtr<FJsonValue>& Input);

	TSharedPtr<FSessionsBranch> Branch;
	TSharedPtr<FSessionsLeaf> Leaf;
};
