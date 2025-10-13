#include "GuardSigner.h"
#include "EcosystemWallet/Primitives/Calls/CallTypedDataFactory.h"
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSapientSignerLeaf.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceSupport.h"

void UGuardSigner::WithHost(const FString& Host)
{
	this->GuardService = NewObject<UGuardService>();
	this->GuardService->WithHost(Host);
}

void UGuardSigner::SignEnvelope(
	const TSharedPtr<FEnvelope>& Envelope,
	const FString& SessionsImageHash,
	const TFunction<void(TSharedPtr<FRSY>)>& OnSuccess,
	const TFunction<void(FString)>& OnFailure)
{
	FEnvelope* EnvelopePtr = Envelope.Get();
	FString DomainJson = FCallTypedDataFactory::FromCalls(EnvelopePtr->Wallet, EnvelopePtr->ChainId, EnvelopePtr->Payload);
	DomainJson = DomainJson.Replace(TEXT("\n"), TEXT(""));
	DomainJson = DomainJson.Replace(TEXT("\t"), TEXT(""));
	
	const TArray<uint8> HashedData = USequenceSupport::EncodeAndHashTypedData(DomainJson);
	const FString HashedHex = FByteArrayUtils::BytesToHexString(HashedData);

	TArray<FGuardSignatureArgs> Signatures;
	for (TSharedPtr<FSignatureOfLeaf> SignatureOfLeaf : EnvelopePtr->Signatures)
	{
		if (SignatureOfLeaf->Type == EConfigSignatureType::SignatureOfSapientSigner)
		{
			auto* SapientSignature = static_cast<FSignatureOfSapientSignerLeaf*>(SignatureOfLeaf.Get());
			Signatures.Add(FGuardSignatureArgs{
				"Sapient",
				SapientSignature->Address,
				SessionsImageHash,
				FByteArrayUtils::BytesToHexString(SapientSignature->Data)
			});	
		}
	}
	
	FSignWithArgs Args {
		TEXT("0x18002Fc09deF9A47437cc64e270843dE094f5984"),
		FSignRequest {
			FCString::Atoi64(*EnvelopePtr->ChainId.Value),
			HashedHex,
			EnvelopePtr->Wallet,
			"Calls",
			FByteArrayUtils::BytesToHexString(FByteArrayUtils::StringToBytes(DomainJson)),
			Signatures
		}
	};

	this->GuardService->SignWith(Args, [OnSuccess](const FSignWithResponse& Response)
	{
		OnSuccess(FRSY::UnpackFrom65(FByteArrayUtils::HexStringToBytes(Response.Sig)));
	}, OnFailure);
}
