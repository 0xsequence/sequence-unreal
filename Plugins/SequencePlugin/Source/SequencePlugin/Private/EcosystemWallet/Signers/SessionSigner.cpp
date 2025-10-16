#include "SessionSigner.h"
#include "Provider.h"
#include "EcosystemWallet/Extensions/ExtensionsFactory.h"
#include "EcosystemWallet/Primitives/Signatures/ExplicitSessionCallSignature.h"
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

void FSessionSigner::IsSupportedCall(const FBigInt& ChainId, const FCall& Call, const TSharedPtr<FSessionsTopology>& SessionsTopology, TFunction<void(bool)> OnCompleted)
{
	if (Credentials.IsExplicit)
	{
		const int32 Permission = FindSupportedPermission(ChainId, Call, SessionsTopology);
		OnCompleted(Permission >= 0);
		return;
	}

	CheckAcceptImplicitRequest(Call, OnCompleted);
}

int32 FSessionSigner::FindSupportedPermission(const FBigInt& ChainId, const FCall& Call, const TSharedPtr<FSessionsTopology>& SessionsTopology)
{
	const TSharedPtr<FSessionPermissions> Permissions = SessionsTopology->GetPermissions(Credentials.SessionAddress);
	
	if (Permissions == nullptr || !Permissions.IsValid())
		return -1;

	const FBigInt PermissionsChainId = FBigInt::FromHex(Permissions->ChainId);
	if (PermissionsChainId.Value != ChainId.Value)
		return -1;

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

	UE_LOG(LogTemp, Display, TEXT("using permission index %d"), PermissionsIndex)
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

void FSessionSigner::GetCurrentUsageLimit(TFunction<void(FBigInt)> OnCompleted)
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

	const FString ValueUsageHash = FByteArrayUtils::BytesToHexString(FSequenceCoder::KeccakHash(FEthAbiBridge::EncodeTwoAddresses(Credentials.SessionAddress, "0xEeeeeEeeeEeEeeEeEeEeeEEEeeeeEeeeeeeeEEeE")));
	const FString Values = FString::Printf(TEXT("[\"%s\",\"%s\"]"), *Credentials.WalletAddress, *ValueUsageHash);
	const FString EncodedFunctionData = USequenceSupport::EncodeFunctionCall("getLimitUsage(address,bytes32)", Values);

	FContractCall Call;
	Call.To = FExtensionsFactory::GetCurrent().Sessions;
	Call.Data = EncodedFunctionData;

	UProvider* Provider = NewObject<UProvider>();
	Provider->Call(Call, EBlockTag::EPending, OnSuccess, OnFailure);
}

void FSessionSigner::CheckAcceptImplicitRequest(const FCall& Call, TFunction<void(bool)> OnCompleted)
{
	UProvider* Provider = NewObject<UProvider>();

	const FString Values = FString::Printf(TEXT("[\"%s\"]"), *Credentials.WalletAddress);
	const FString EncodedFunctionData = USequenceSupport::EncodeFunctionCall(
		"acceptImplicitRequest(address,(address,bytes4,bytes32,bytes32,bytes,(string,uint64)),(address,uint256,bytes,uint256,bool,bool,uint256))",
		Values);

	FContractCall ContractCall;
	ContractCall.To = Call.To;
	ContractCall.Data = EncodeAcceptImplicitRequest(Call);
	
	Provider->Call(ContractCall, EBlockTag::EPending, [this, OnCompleted](const FString& Response)
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

FString FSessionSigner::EncodeAcceptImplicitRequest(const FCall& Call)
{
	// -----------------------
    // Construct attestationData tuple
    // -----------------------
	
    const FString ApprovedSigner = Credentials.Attestation.ApprovedSigner;
    const FString IdentityType   = FString::Printf(TEXT("%s"), *FByteArrayUtils::BytesToHexString(Credentials.Attestation.IdentityType));
    const FString IssuerHash     = FString::Printf(TEXT("%s"), *FByteArrayUtils::BytesToHexString(Credentials.Attestation.IssuerHash));
    const FString AudienceHash   = FString::Printf(TEXT("%s"), *FByteArrayUtils::BytesToHexString(Credentials.Attestation.AudienceHash));
    const FString ApplicationData = FString::Printf(TEXT("%s"), *FByteArrayUtils::BytesToHexString(Credentials.Attestation.ApplicationData));
    const FString RedirectUrl    = Credentials.Attestation.AuthData.RedirectUrl;
    const FString IssuedAt       = FString::Printf(TEXT("%s"), *Credentials.Attestation.AuthData.IssuedAt.Value);

    // Solidity nested tuple as JSON array
	
    const FString AttestationData = FString::Printf(
        TEXT("[\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",[\"%s\",\"%s\"]]"),
        *ApprovedSigner,
        *IdentityType,
        *IssuerHash,
        *AudienceHash,
        *ApplicationData,
        *RedirectUrl,
        *IssuedAt
    );

    // -----------------------
    // Construct callData tuple
    // -----------------------
	
    const FString CallTo           = Call.To;
    const FString CallValue        = FString::Printf(TEXT("%s"), *Call.Value.Value);
    const FString CallDataHex      = FString::Printf(TEXT("%s"), *Call.Data);
    const FString CallGasLimit     = FString::Printf(TEXT("%s"), *Call.GasLimit.Value);
    const FString CallDelegate     = Call.DelegateCall ? TEXT("true") : TEXT("false");
    const FString CallOnlyFallback = Call.OnlyFallback ? TEXT("true") : TEXT("false");
    const FString CallBehavior     = FString::Printf(TEXT("%s"), *Call.BehaviorOnError);

    const FString CallDataJson = FString::Printf(
        TEXT("[\"%s\",\"%s\",\"%s\",\"%s\",%s,%s,\"%s\"]"),
        *CallTo,
        *CallValue,
        *CallDataHex,
        *CallGasLimit,
        *CallDelegate,
        *CallOnlyFallback,
        *CallBehavior
    );

    // -----------------------
    // Combine full argument array for ABI encoding
    // -----------------------
	
    const FString FullArgs = FString::Printf(
        TEXT("[\"%s\",%s,%s]"),
        *Credentials.SessionAddress,
        *AttestationData,
        *CallDataJson
    );

    // -----------------------
    // Encode function call
    // -----------------------
	
    const FString Encoded = USequenceSupport::EncodeFunctionCall(
        TEXT("acceptImplicitRequest(address,(address,bytes4,bytes32,bytes32,bytes,(string,uint64)),(address,uint256,bytes,uint256,bool,bool,uint256))"),
        FullArgs
    );

	return Encoded;
}
