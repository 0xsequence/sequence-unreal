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

void FSessionSigner::IsSupportedCall(const FCall& Call, TFunction<void(bool)> OnCompleted)
{
	OnCompleted(true);
}

uint32 FSessionSigner::FindSupportedPermission(const FCall& Call, const TSharedPtr<FSessionsTopology>& SessionsTopology)
{
	return 0;
}

TSharedPtr<FSessionCallSignature> FSessionSigner::SignCall(const FBigInt& ChainId, const FCall& Call, const int32& CallIdx,
	const TSharedPtr<FSessionsTopology>& SessionsTopology, const FBigInt& Space, const FBigInt& Nonce)
{
	TArray<uint8> HashedCall = HashCallWithReplayProtection(ChainId, Call, CallIdx, Space, Nonce);
	UE_LOG(LogTemp, Display, TEXT("hashedCall %s"), *FByteArrayUtils::BytesToHexString(HashedCall));
	TArray<uint8> SignedCall = FEthAbiBridge::SignRecoverable(HashedCall, FByteArrayUtils::HexStringToBytes(Credentials.PrivateKey));

	TSharedPtr<FRSY> Signature = FRSY::UnpackFrom65(SignedCall);
	
	return MakeShared<FExplicitSessionCallSignature>(0, Signature);
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

