#include "ImplicitRequestEncoder.h"

#include "EcosystemWallet/Primitives/Attestation/Attestation.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceSupport.h"

FString FImplicitRequestEncoder::EncodeAcceptImplicitRequest(const FCall& Call, const FString& ParentAddress, const FAttestation& Attestation)
{
	// -----------------------
	// Construct attestationData tuple
	// -----------------------
	
	const FString ApprovedSigner = Attestation.ApprovedSigner;
	const FString IdentityType   = FString::Printf(TEXT("%s"), *FByteArrayUtils::BytesToHexString(FByteArrayUtils::PadLeft(Attestation.IdentityType, 4)));
	const FString IssuerHash     = FString::Printf(TEXT("%s"), *FByteArrayUtils::BytesToHexString(FByteArrayUtils::PadLeft(Attestation.IssuerHash, 32)));
	const FString AudienceHash   = FString::Printf(TEXT("%s"), *FByteArrayUtils::BytesToHexString(FByteArrayUtils::PadLeft(Attestation.AudienceHash, 32)));
	const FString ApplicationData = FString::Printf(TEXT("%s"), *FByteArrayUtils::BytesToHexString(Attestation.ApplicationData));
	const FString RedirectUrl    = Attestation.AuthData.RedirectUrl;
	const FString IssuedAt       = FString::Printf(TEXT("%s"), *Attestation.AuthData.IssuedAt.Value);

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
		*ParentAddress,
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

	UE_LOG(LogTemp, Display, TEXT("FullArgs %s"), *FullArgs);
	UE_LOG(LogTemp, Display, TEXT("Encoded %s"), *Encoded);

	return Encoded;
}
