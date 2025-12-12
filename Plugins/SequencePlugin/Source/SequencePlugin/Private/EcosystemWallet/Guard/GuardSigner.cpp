#include "GuardSigner.h"

#include "EcosystemWallet/Extensions/ExtensionsFactory.h"
#include "EcosystemWallet/Guard/Models/SignWith.h"
#include "EcosystemWallet/KeyMachine/Models/ConfigUpdatesResponse.h"
#include "EcosystemWallet/Primitives/Calls/CallTypedDataFactory.h"
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSapientSignerLeaf.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSignerLeafHash.h"
#include "Util/ByteArrayUtils.h"
#include "Util/SequenceSupport.h"

void UGuardSigner::SetConfig(const FGuardConfig& Config)
{
	this->GuardService = NewObject<UGuardService>();
	this->GuardService->WithHost(Config.Url);
	this->GuardSigner = GetGuardSignerAddress(Config);
}

void UGuardSigner::SignEnvelope(
	const TSharedPtr<FEnvelope>& Envelope,
	const TArray<FConfigUpdate>& ConfigUpdates,
	const FString& SessionsImageHash,
	const TFunction<void(TSharedPtr<FEnvelope>, const TArray<FConfigUpdate>, TSharedPtr<FSignatureOfSignerLeafHash>)>& OnSuccess,
	const TFunction<void(FString)>& OnFailure)
{
	FString DomainJson = FCallTypedDataFactory::FromCalls(Envelope->Wallet, Envelope->ChainId, Envelope->Payload);
	DomainJson = DomainJson.Replace(TEXT("\n"), TEXT(""));
	DomainJson = DomainJson.Replace(TEXT("\t"), TEXT(""));
	
	const TArray<uint8> HashedData = USequenceSupport::EncodeAndHashTypedData(DomainJson);
	const FString HashedHex = FByteArrayUtils::BytesToHexString(HashedData);

	TArray<FGuardSignatureArgs> Signatures;
	for (TSharedPtr<FSignatureOfLeaf> SignatureOfLeaf : Envelope->Signatures)
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
		this->GuardSigner,
		FSignRequest {
			FCString::Atoi64(*Envelope->ChainId.Value),
			HashedHex,
			Envelope->Wallet,
			"Calls",
			FByteArrayUtils::BytesToHexString(FByteArrayUtils::StringToBytes(DomainJson)),
			Signatures
		}
	};

	this->GuardService->SignWith(Args, [this, Envelope, ConfigUpdates, OnSuccess](const FSignWithResponse& Response)
	{
		const TSharedPtr<FRSY> Signature = FRSY::UnpackFrom65(FByteArrayUtils::HexStringToBytes(Response.Sig));
		const TSharedPtr<FSignatureOfSignerLeafHash> SignatureOfSignerLeafHash = MakeShared<FSignatureOfSignerLeafHash>(FSignatureOfSignerLeafHash());
		SignatureOfSignerLeafHash->Address = this->GuardSigner;
		SignatureOfSignerLeafHash->Signature = Signature;
		
		OnSuccess(Envelope, ConfigUpdates, SignatureOfSignerLeafHash);
	}, OnFailure);
}

FString UGuardSigner::GetGuardSignerAddress(const FGuardConfig& Config)
{
	TSet<FString> SetOther;

	for (const FString& Signer : FExtensionsFactory::GetCurrent().GuardSigners)
	{
		SetOther.Add(Signer);
	}

	for (const TPair<FString, FString>& Pair : Config.ModuleAddresses)
	{
		const FString& Value = Pair.Value;

		if (SetOther.Contains(Value))
		{
			return Value;
		}
	}

	return FString();
}
