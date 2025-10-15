#include "SessionSigner.h"
#include "EcosystemWallet/Primitives/Signatures/ExplicitSessionCallSignature.h"
#include "Util/SequenceCoder.h"

FString FSessionSigner::GetIdentitySigner()
{
	if (Credentials.IsExplicit)
	{
		return "";
	}

	return "";
}

void FSessionSigner::IsSupportedCall(const FBigInt& ChainId, const FCall& Call, const TSharedPtr<FSessionsTopology>& SessionsTopology, TFunction<void(bool)> OnCompleted)
{
	const int32 Permission = FindSupportedPermission(ChainId, Call, SessionsTopology);
	OnCompleted(Permission >= 0);
}

int32 FSessionSigner::FindSupportedPermission(const FBigInt& ChainId, const FCall& Call, const TSharedPtr<FSessionsTopology>& SessionsTopology)
{
	const TSharedPtr<FSessionPermissions> Permissions = SessionsTopology->GetPermissions(Credentials.SessionAddress);
	
	if (Permissions == nullptr || !Permissions.IsValid())
		return -1;

	const FBigInt PermissionsChainId = FBigInt::FromHex(Permissions->ChainId);
	UE_LOG(LogTemp, Display, TEXT("ChainId %s %s %s"), *Permissions->ChainId, *PermissionsChainId.Value, *ChainId.Value)
	if (PermissionsChainId.Value != ChainId.Value)
		return -1;

	UE_LOG(LogTemp, Display, TEXT("Permissions length %d"), Permissions->Permissions.Num())

	int32 PermissionsIndex = -1;
	for (int i = 0; i < Permissions->Permissions.Num(); ++i)
	{
		FPermission Permission = Permissions->Permissions[i];
		UE_LOG(LogTemp, Display, TEXT("Target %s, To %s"), *Permission.Target, *Call.To)
		if (Permission.Target.Equals(Call.To, ESearchCase::IgnoreCase))
		{
			PermissionsIndex = i;
			break;
		}
	}
	
	return PermissionsIndex;
}

TSharedPtr<FSessionCallSignature> FSessionSigner::SignCall(const FBigInt& ChainId, const FCall& Call, const int32& CallIdx,
	const TSharedPtr<FSessionsTopology>& SessionsTopology, const FBigInt& Space, const FBigInt& Nonce)
{
	TArray<uint8> HashedCall = HashCallWithReplayProtection(ChainId, Call, CallIdx, Space, Nonce);
	TArray<uint8> SignedCall = FEthAbiBridge::SignRecoverable(HashedCall, FByteArrayUtils::HexStringToBytes(Credentials.PrivateKey));

	TSharedPtr<FRSY> Signature = FRSY::UnpackFrom65(SignedCall);

	const int32 PermissionIndex = FindSupportedPermission(ChainId, Call, SessionsTopology);
	if (PermissionIndex < 0)
	{
		return nullptr;
	}
	
	return MakeShared<FExplicitSessionCallSignature>(PermissionIndex, Signature);
}

TArray<uint8> FSessionSigner::HashCallWithReplayProtection(const FBigInt& ChainId, const FCall& Call, const uint32& CallIdx, const FBigInt& Space, const FBigInt& Nonce)
{
	TArray<uint8> ChainBytes = FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(ChainId.Value), 32);
	TArray<uint8> SpaceBytes = FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(Space.Value), 32);
	TArray<uint8> NonceBytes = FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(Nonce.Value), 32);
	TArray<uint8> CallIdxBytes = FByteArrayUtils::PadLeft(FByteArrayUtils::ByteArrayFromNumber(CallIdx, FByteArrayUtils::MinBytesFor(CallIdx)), 32);
	TArray<uint8> CallHashBytes = Call.Hash();

	const TArray<uint8> Concatenated = FByteArrayUtils::ConcatBytes(
		{ ChainBytes, SpaceBytes, NonceBytes, CallIdxBytes, CallHashBytes });

	return FSequenceCoder::KeccakHash(Concatenated);
}

