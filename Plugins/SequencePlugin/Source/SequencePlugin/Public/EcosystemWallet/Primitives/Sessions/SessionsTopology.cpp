#include "SessionsTopology.h"
#include "Leafs/SessionsIdentitySignerLeaf.h"
#include "Leafs/SessionsImplicitBlacklistLeaf.h"
#include "Leafs/SessionsPermissionsLeaf.h"
#include "Util/ByteArrayUtils.h"

FString FSessionsTopology::FindIdentitySigner() const
{
	if (IsBranch())
	{
		for (TSharedPtr<FSessionsTopology> Child : Branch->Children)
		{
			FString ChildResult = Child.Get()->FindIdentitySigner();
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
