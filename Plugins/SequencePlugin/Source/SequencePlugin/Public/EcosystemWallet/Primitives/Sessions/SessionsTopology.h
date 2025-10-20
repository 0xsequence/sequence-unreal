#pragma once
#include "SessionsBranch.h"
#include "SessionsLeaf.h"
#include "Dom/JsonValue.h"
#include "EcosystemWallet/Primitives/Permission/SessionPermissions.h"

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

	FString GetIdentitySigner() const;
	TArray<FString> GetImplicitBlacklist() const;
	TSharedPtr<FSessionPermissions> GetPermissions(const FString& Address) const;
	TArray<FString> GetExplicitSigners() const;
	TArray<FString> GetExplicitSigners(const TArray<FString>& Current) const;
	TSharedPtr<FSessionsTopology> Minimise(const TArray<FString>& ExplicitSigners, const TArray<FString>& ImplicitSigners, const FString& IdentitySigner) const;
	TArray<uint8> Encode() const;
	TArray<uint8> Hash(const bool Raw) const;
	
	static TSharedPtr<FSessionsTopology> FromServiceConfigTree(const TSharedPtr<FJsonValue>& Input);
	static TSharedPtr<FSessionsLeaf> DecodeLeaf(const FString& Input);

	TSharedPtr<FSessionsBranch> Branch;
	TSharedPtr<FSessionsLeaf> Leaf;

	static constexpr uint8 FlagPermissions = 0;
	static constexpr uint32 FlagNode = 1;
	static constexpr uint8 FlagBranch = 2;
	static constexpr uint8 FlagBlacklist = 3;
	static constexpr uint8 FlagIdentitySigner = 4;
};
