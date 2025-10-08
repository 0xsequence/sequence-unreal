#include "SessionsTopology.h"
#include "Leafs/SessionsIdentitySignerLeaf.h"
#include "Leafs/SessionsImplicitBlacklistLeaf.h"
#include "Leafs/SessionsPermissionsLeaf.h"
#include "Util/ByteArrayUtils.h"

TSharedPtr<FSessionsLeaf> FSessionsTopology::FindIdentitySigner(const FString& Address) const
{
	return MakeShareable(new FSessionsLeaf);
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
	
	return MakeShareable(new FSessionsTopology());
}

TSharedPtr<FSessionsLeaf> FSessionsTopology::DecodeLeaf(const FString& Value)
{
	TArray<uint8> Data;
	if (FByteArrayUtils::HexStringToBytes(Value, Data))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to convert hex to byte array"));
		return nullptr;
	}

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
