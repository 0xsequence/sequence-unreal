#include "SessionsTopology.h"
#include "Leafs/SessionsIdentitySignerLeaf.h"
#include "Leafs/SessionsImplicitBlacklistLeaf.h"
#include "Leafs/SessionsNodeLeaf.h"
#include "Leafs/SessionsPermissionsLeaf.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceCoder.h"

FString FSessionsTopology::GetIdentitySigner() const
{
	if (IsBranch())
	{
		for (TSharedPtr<FSessionsTopology> Child : Branch->Children)
		{
			FString ChildResult = Child.Get()->GetIdentitySigner();
			if (ChildResult != "")
			{
				return ChildResult;
			}
		}

		return "";
	}

	if (IsLeaf() && Leaf->Type == ESessionsLeafType::IdentitySigner)
	{
		auto* IdentitySignerLeaf = static_cast<FSessionsIdentitySignerLeaf*>(Leaf.Get());
		return IdentitySignerLeaf->IdentitySigner;
	}

	return "";
}

TArray<FString> FSessionsTopology::GetImplicitBlacklist() const
{
	if (IsBranch())
	{
		for (TSharedPtr<FSessionsTopology> Child : Branch->Children)
		{
			TArray<FString> ChildResult = Child.Get()->GetImplicitBlacklist();
			if (ChildResult.Num() > 0)
			{
				return ChildResult;
			}
		}

		return TArray<FString>();
	}

	if (IsLeaf() && Leaf->Type == ESessionsLeafType::ImplicitBlacklist)
	{
		auto* IdentitySignerLeaf = static_cast<FSessionsImplicitBlacklistLeaf*>(Leaf.Get());
		return IdentitySignerLeaf->Blacklist;
	}

	return TArray<FString>();
}

TArray<FString> FSessionsTopology::GetExplicitSigners() const
{
	TArray<FString> Empty;
	return GetExplicitSigners(Empty);
}

TArray<FString> FSessionsTopology::GetExplicitSigners(const TArray<FString>& Current) const
{
	TArray<FString> Result = Current;
	
	if (IsBranch())
	{
		for (TSharedPtr<FSessionsTopology> Child : Branch->Children)
		{
			TArray<FString> ChildResult = Child.Get()->GetExplicitSigners(Current);
			if (ChildResult.Num() > 0)
			{
				Result.Append(ChildResult);
			}
		}

		return Result;
	}

	if (IsLeaf() && Leaf->Type == ESessionsLeafType::Permissions)
	{
		auto* PermissionsLeaf = static_cast<FSessionsPermissionsLeaf*>(Leaf.Get());
		Result.Add(PermissionsLeaf->Permissions.SessionAddress);
		return Result;
	}

	return Result;
}

TSharedPtr<FSessionsTopology> FSessionsTopology::Minimise(const TArray<FString>& ExplicitSigners, const TArray<FString>& ImplicitSigners, const FString& IdentitySigner) const
{
	if (IsBranch())
    {
        TArray<TSharedPtr<FSessionsTopology>> BranchList;
        BranchList.Reserve(Branch->Children.Num());

        for (TSharedPtr<FSessionsTopology> Child : Branch->Children)
        {
            TSharedPtr<FSessionsTopology> MinChild = Child->Minimise(ExplicitSigners, ImplicitSigners, IdentitySigner);
            BranchList.Add(MinChild);
        }

        bool bAllLeaves = true;
        for (TSharedPtr<FSessionsTopology> Child : BranchList)
        {
            if (!(Child->IsLeaf() && Child->Leaf->Type == ESessionsLeafType::SessionsNode))
            {
                bAllLeaves = false;
                break;
            }
        }

        if (bAllLeaves)
        {
            TArray<TArray<uint8>> NodeValues;
            for (TSharedPtr<FSessionsTopology> Child : BranchList)
            {
            	if (Child->IsLeaf() && Child->Leaf->Type == ESessionsLeafType::SessionsNode)
            	{
            		FSessionsNodeLeaf* LeafNode = static_cast<FSessionsNodeLeaf*>(Child->Leaf.Get());
            		if (!LeafNode)
            			continue;
            	
            		NodeValues.Add(FByteArrayUtils::HexStringToBytes(LeafNode->Value));
            	}
            }

            TArray<uint8> Concatenated = FByteArrayUtils::ConcatBytes(NodeValues);
            TArray<uint8> Hashed = FSequenceCoder::KeccakHash(Concatenated);

            return MakeShared<FSessionsTopology>(FSessionsTopology(MakeShared<FSessionsNodeLeaf>(FSessionsNodeLeaf(FByteArrayUtils::BytesToHexString(Hashed)))));
        }

		return MakeShared<FSessionsTopology>(FSessionsTopology(MakeShared<FSessionsBranch>(FSessionsBranch(BranchList))));
    }

    if (Leaf->Type == ESessionsLeafType::Permissions)
    {
    	FSessionsPermissionsLeaf* PermissionLeaf = static_cast<FSessionsPermissionsLeaf*>(Leaf.Get());
        if (ExplicitSigners.Contains(PermissionLeaf->Permissions.SessionAddress))
        {
        	return MakeShared<FSessionsTopology>(*this);
        }

    	return MakeShared<FSessionsTopology>(FSessionsTopology(MakeShared<FSessionsNodeLeaf>(FSessionsNodeLeaf(FByteArrayUtils::BytesToHexString(Hash(true))))));
    }

    if (Leaf->Type == ESessionsLeafType::ImplicitBlacklist)
    {
    	FSessionsImplicitBlacklistLeaf* BlacklistLeaf = static_cast<FSessionsImplicitBlacklistLeaf*>(Leaf.Get());
        if (ImplicitSigners.Num() == 0)
        {
        	return MakeShared<FSessionsTopology>(FSessionsTopology(MakeShared<FSessionsNodeLeaf>(FSessionsNodeLeaf(FByteArrayUtils::BytesToHexString(Hash(true))))));
        }

        return MakeShared<FSessionsTopology>(*this);
    }

    if (Leaf->Type == ESessionsLeafType::IdentitySigner)
    {
    	FSessionsIdentitySignerLeaf* IdentitySignerLeaf = static_cast<FSessionsIdentitySignerLeaf*>(Leaf.Get());
        if (IdentitySignerLeaf->IdentitySigner.Equals(IdentitySigner, ESearchCase::IgnoreCase))
        {
        	return MakeShared<FSessionsTopology>(*this);
        }

        return MakeShared<FSessionsTopology>(FSessionsTopology(MakeShared<FSessionsNodeLeaf>(FSessionsNodeLeaf(FByteArrayUtils::BytesToHexString(Hash(true))))));
    }

    if (Leaf->Type == ESessionsLeafType::SessionsNode)
    {
    	return MakeShared<FSessionsTopology>(*this);
    }

    UE_LOG(LogTemp, Error, TEXT("Invalid topology"));
	return MakeShared<FSessionsTopology>(*this);
}

TArray<uint8> FSessionsTopology::Encode() const
{
	if (IsBranch())
	{
		TArray<TArray<uint8>> EncodedChildren;
		for (TSharedPtr<FSessionsTopology> SessionsTopology : Branch.Get()->Children)
		{
			EncodedChildren.Add(SessionsTopology->Encode());
		}

		const TArray<uint8> ChildrenBytes = FByteArrayUtils::ConcatBytes(EncodedChildren);
		const uint64 EncodedSize = FByteArrayUtils::MinBytesFor(ChildrenBytes.Num());
		const int32 Flag = FSessionsTopology::FlagBranch << 4 | EncodedSize;

		return FByteArrayUtils::ConcatBytes({
			FByteArrayUtils::ByteArrayFromNumber(Flag, 1),
			FByteArrayUtils::ByteArrayFromNumber(ChildrenBytes.Num(), EncodedSize),
			ChildrenBytes
		});
	}

	if (IsLeaf())
	{
		return Leaf.Get()->Encode();
	}

	return TArray<uint8>();
}

TArray<uint8> FSessionsTopology::Hash(const bool Raw) const
{
	if (IsBranch())
	{
		TArray<TSharedPtr<FSessionsTopology>> Children = Branch.Get()->Children;
		if (Children.Num() == 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Empty branch"));
			throw std::runtime_error("Empty branch");
		}

		TArray<TArray<uint8>> HashedChildren;
		HashedChildren.Reserve(Children.Num());
		for (TSharedPtr<FSessionsTopology> Child : Children)
		{
			HashedChildren.Add(Child->Hash(Raw));
		}

		TArray<uint8> ChildBytes = HashedChildren[0];

		for (int32 i = 1; i < HashedChildren.Num(); ++i)
		{
			TArray<uint8> Concatenated = FByteArrayUtils::ConcatBytes({
				ChildBytes,
				HashedChildren[i]
			});

			ChildBytes = FSequenceCoder::KeccakHash(Concatenated);
		}

		return ChildBytes;
	}

	if (IsLeaf())
	{
		TArray<uint8> EncodedLeaf;

		if (Raw)
		{
			EncodedLeaf = Leaf->EncodeForHash();
		}
		else
		{
			EncodedLeaf = Leaf->Encode();
		}
		
		return FSequenceCoder::KeccakHash(EncodedLeaf);
	}

	UE_LOG(LogTemp, Error, TEXT("Invalid tree structure"));
	return TArray<uint8>();
}

TSharedPtr<FSessionsTopology> FSessionsTopology::FromServiceConfigTree(const TSharedPtr<FJsonValue>& Input)
{
	if (Input->Type == EJson::Array)
	{
		TArray<TSharedPtr<FJsonValue>> List = Input->AsArray();
		if (List.Num() < 2)
		{
			return MakeShareable(new FSessionsTopology());
		}

		TArray<TSharedPtr<FSessionsTopology>> Children = TArray<TSharedPtr<FSessionsTopology>>();
		for (int32 i = 0; i < List.Num(); i++)
		{
			Children.Add(FromServiceConfigTree(List[i]));
		}
        
		return MakeShared<FSessionsTopology>(FSessionsTopology(MakeShared<FSessionsBranch>(Children)));
	}

	if (Input->Type != EJson::Object)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid JSON object"));
		return MakeShared<FSessionsTopology>(FSessionsTopology());
	}

	const TSharedPtr<FJsonObject> JsonObject = Input->AsObject();
	if (JsonObject->HasField(TEXT("data")))
	{
		const FString DataValue = JsonObject->GetStringField(TEXT("data"));
		return MakeShareable(new FSessionsTopology(DecodeLeaf(DataValue)));
	}

	return MakeShared<FSessionsTopology>(FSessionsTopology());
}

TSharedPtr<FSessionsLeaf> FSessionsTopology::DecodeLeaf(const FString& Value)
{
	TArray<uint8> Data = FByteArrayUtils::HexStringToBytes(Value);
	uint8 Flag = Data[0];

	if (Flag == FlagBlacklist)
	{
		TArray<FString> Blacklist;
		for (int32 i = 1; i < Data.Num(); i += 20)
		{
			if (i + 20 > Data.Num())
				break;

			TArray<uint8> Slice;
			Slice.Append(&Data[i], 20);
			FString Hex = FByteArrayUtils::BytesToHexString(Slice);
			Blacklist.Add(Hex);
		}

		return MakeShared<FSessionsImplicitBlacklistLeaf>(FSessionsImplicitBlacklistLeaf(Blacklist));
	}

	if (Flag == FlagIdentitySigner)
	{
		if (Data.Num() < 21)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid identity signer data"));
			return nullptr;
		}

		TArray<uint8> Slice;
		Slice.Append(&Data[1], 20);
		const FString IdentitySignerValue = FByteArrayUtils::BytesToHexString(Slice);
		
		return MakeShared<FSessionsIdentitySignerLeaf>(FSessionsIdentitySignerLeaf(IdentitySignerValue));
	}

	if (Flag == FlagPermissions)
	{
		TArray<uint8> Slice;
		Slice.Append(&Data[1], Data.Num() - 1);

		FSessionPermissions Permissions = FSessionPermissions::Decode(Slice);
		return MakeShared<FSessionsPermissionsLeaf>(FSessionsPermissionsLeaf(Permissions));
	}

	UE_LOG(LogTemp, Error, TEXT("Invalid leaf"));
	return nullptr;
}
