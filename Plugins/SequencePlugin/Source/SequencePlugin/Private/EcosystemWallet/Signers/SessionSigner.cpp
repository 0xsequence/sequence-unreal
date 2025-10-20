#include "SessionSigner.h"
#include "Provider.h"
#include "EcosystemWallet/Extensions/ExtensionsFactory.h"
#include "EcosystemWallet/Primitives/Signatures/ExplicitSessionCallSignature.h"
#include "EcosystemWallet/Primitives/Signatures/ImplicitSessionCallSignature.h"
#include "EcosystemWallet/Signers/ImplicitRequestEncoder.h"
#include "Math/BigInt.h"
#include "Util/SequenceCoder.h"
#include "Util/SequenceSupport.h"

FString FSessionSigner::GetIdentitySigner()
{
	if (Credentials.IsExplicit)
		return "";

	TArray<uint8> OutPublicKey;
	TArray<uint8> OutAddress;

	const bool Result = USequenceSupport::RecoverEthPubAndAddress(
		Credentials.Signature.Pack65(),
		Credentials.Attestation.Hash(),
		OutPublicKey,
		OutAddress);

	if (!Result)
		return "";
	
	return FByteArrayUtils::BytesToHexString(OutAddress);
}

FString FSessionSigner::GetSessionAddress() const
{
	if (Credentials.SessionAddress != "")
		return Credentials.SessionAddress;

	UE_LOG(LogTemp, Error, TEXT("SessionSigner credentials are invalid"));
	return "";
}

bool FSessionSigner::IsSupportedCall(const FBigInt& ChainId, const FCall& Call, const TArray<FSessionSigner>& Signers, const TSharedPtr<FSessionsTopology>& SessionsTopology)
{
	if (Credentials.IsExplicit)
	{
		if (Credentials.ChainId != ChainId.Value)
			return false;
		
		if (CheckCallForIncrementUsageLimit(Call))
			return true;
		
		const int32 PermissionIndex = FindSupportedPermission(ChainId, Call, SessionsTopology);
		return PermissionIndex >= 0;
	}

	for (FSessionSigner Signer : Signers)
	{
		if (Signer.Credentials.IsExplicit && Signer.IsSupportedCall(ChainId, Call, Signers, SessionsTopology))
		{
			return false;
		}
	}

	return true;
}

int32 FSessionSigner::FindSupportedPermission(const FBigInt& ChainId, const FCall& Call, const TSharedPtr<FSessionsTopology>& SessionsTopology)
{
	const TSharedPtr<FSessionPermissions> Permissions = SessionsTopology->GetPermissions(GetSessionAddress());
	
	if (Permissions == nullptr || !Permissions.IsValid())
	{
		UE_LOG(LogTemp, Display, TEXT("Null permissions for address %s"), *GetSessionAddress())
		return -1;
	}

	const FBigInt PermissionsChainId = FBigInt::FromHex(Permissions->ChainId);
	if (PermissionsChainId.Value != ChainId.Value)
	{
		UE_LOG(LogTemp, Display, TEXT("Invalid chain id"))
		return -1;
	}

	int32 PermissionsIndex = -1;
	for (int i = 0; i < Permissions->Permissions.Num(); ++i)
	{
		FPermission Permission = Permissions->Permissions[i];
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

	if (Credentials.IsExplicit)
	{
		int32 PermissionIndex = 0;
		if (!CheckCallForIncrementUsageLimit(Call))
		{
			PermissionIndex = FindSupportedPermission(ChainId, Call, SessionsTopology);
			if (PermissionIndex < 0)
			{
				return nullptr;
			}	
		}
	
		return MakeShared<FExplicitSessionCallSignature>(PermissionIndex, Signature);
	}
	
	return MakeShared<FImplicitSessionCallSignature>(
		MakeShared<FAttestation>(Credentials.Attestation),
		MakeShared<FRSY>(Credentials.Signature),
		Signature);
}

void FSessionSigner::PrepareIncrements(
	const FBigInt& ChainId,
	const TArray<FCall>& Calls,
	const TSharedPtr<FSessionsTopology>& SessionsTopology,
	const TFunction<void(TSharedPtr<FUsageLimit>)>& OnCompleted)
{
	TArray<FCall> ValidCalls;
	for (FCall Call1 : Calls)
	{
		const int32 PermissionIndex = FindSupportedPermission(ChainId, Call1, SessionsTopology);
		UE_LOG(LogTemp, Display, TEXT("PermissionIndex %d"), PermissionIndex);
		if (PermissionIndex < 0)
			continue;

		ValidCalls.Add(Call1);
	}
	
	const FString ValueUsageHash = GetValueUsageHash();
	GetCurrentUsageLimit(ValueUsageHash, [this, ValueUsageHash, ChainId, ValidCalls, SessionsTopology, OnCompleted](const FBigInt& CurrentUsageLimit)
	{
		FBigInt ValueUsed = FBigInt(CurrentUsageLimit.Value);
		for (FCall Call : ValidCalls)
		{
			ValueUsed.Add(Call.Value);
			UE_LOG(LogTemp, Display, TEXT("Used 2 %s, %s"), *Call.Value.Value, *ValueUsed.Value);
		}
		
		if (ValueUsed.Value == "0")
		{
			OnCompleted(nullptr);
			return;
		}

		const FUsageLimit UsageLimit = FUsageLimit
		{
			ValueUsageHash,
			ValueUsed
		};
		
		OnCompleted(MakeShared<FUsageLimit>(UsageLimit));
	});
}

TArray<uint8> FSessionSigner::HashCallWithReplayProtection(const FBigInt& ChainId, const FCall& Call, const uint32& CallIdx, const FBigInt& Space, const FBigInt& Nonce)
{
	UE_LOG(LogTemp, Log, TEXT("FSessionSigner::HashCallWithReplayProtection %s %s %s %d"), *ChainId.Value, *Space.Value, *Nonce.Value, CallIdx);
	TArray<uint8> ChainBytes = FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(ChainId.Value), 32);
	TArray<uint8> SpaceBytes = FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(Space.Value), 32);
	TArray<uint8> NonceBytes = FByteArrayUtils::PadLeft(FEthAbiBridge::EncodeBigInteger(Nonce.Value), 32);
	TArray<uint8> CallIdxBytes = FByteArrayUtils::PadLeft(FByteArrayUtils::ByteArrayFromNumber(CallIdx, FByteArrayUtils::MinBytesFor(CallIdx)), 32);
	TArray<uint8> CallHashBytes = Call.Hash();

	const TArray<uint8> Concatenated = FByteArrayUtils::ConcatBytes(
		{ ChainBytes, SpaceBytes, NonceBytes, CallIdxBytes, CallHashBytes });

	return FSequenceCoder::KeccakHash(Concatenated);
}

FString FSessionSigner::GenerateImplicitRequestMagic()
{
	return FByteArrayUtils::BytesToHexString(
		FSequenceCoder::KeccakHash(
		FByteArrayUtils::ConcatBytes({
			FSequenceCoder::KeccakHash(FByteArrayUtils::StringToBytes("acceptImplicitRequest")),
			FByteArrayUtils::HexStringToBytes(Credentials.WalletAddress),
			Credentials.Attestation.AudienceHash,
			Credentials.Attestation.IssuerHash
		})));
}

void FSessionSigner::GetCurrentUsageLimit(const FString& ValueUsageHash, TFunction<void(FBigInt)> OnCompleted)
{
	const TSuccessCallback<FString> OnSuccess = [this, OnCompleted](const FString& Response)
	{
		OnCompleted(FBigInt::FromHex(Response));
	};

	const FFailureCallback OnFailure = [this, OnCompleted](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error while getting current usage limit: %s"), *Error.Message);
		OnCompleted(FBigInt("0"));
	};
	
	const FString Values = FString::Printf(TEXT("[\"%s\",\"%s\"]"), *Credentials.WalletAddress, *ValueUsageHash);
	const FString EncodedFunctionData = USequenceSupport::EncodeFunctionCall("getLimitUsage(address,bytes32)", Values);

	FContractCall Call;
	Call.To = FExtensionsFactory::GetCurrent().Sessions;
	Call.Data = EncodedFunctionData;

	UProvider* Provider = NewObject<UProvider>();
	Provider->Call(Call, EBlockTag::EPending, OnSuccess, OnFailure);
}

bool FSessionSigner::CheckCallForIncrementUsageLimit(const FCall& Call)
{
	return FByteArrayUtils::BytesToHexString(FByteArrayUtils::SliceBytes(FByteArrayUtils::HexStringToBytes(Call.Data), 0, 4)) == "0x42de1418";
}

FString FSessionSigner::GetValueUsageHash()
{
	return FByteArrayUtils::BytesToHexString(
		FSequenceCoder::KeccakHash(
			FEthAbiBridge::EncodeTwoAddresses(
				GetSessionAddress(),
				"0xEeeeeEeeeEeEeeEeEeEeeEEEeeeeEeeeeeeeEEeE")));
}

void FSessionSigner::CheckAcceptImplicitRequest(const FCall& Call, TFunction<void(bool)> OnCompleted)
{
	UProvider* Provider = NewObject<UProvider>();

	FContractCall ContractCall;
	ContractCall.To = Call.To;
	ContractCall.Data = FImplicitRequestEncoder::EncodeAcceptImplicitRequest(Call, Credentials.WalletAddress, Credentials.Attestation);
	
	Provider->Call(ContractCall, EBlockTag::ELatest, [this, OnCompleted](const FString& Response)
	{
		const FString ExpectedResult = GenerateImplicitRequestMagic();
		const bool AcceptedImplicitRequest = Response.Equals(ExpectedResult, ESearchCase::IgnoreCase);
		OnCompleted(AcceptedImplicitRequest);
	},
	[OnCompleted](const FSequenceError& Error)
	{
		OnCompleted(false);
	});
}
