#include "SignatureService.h"
#include "SignerService.h"
#include "Extensions/ExtensionsFactory.h"
#include "Guard/GuardSigner.h"
#include "Signatures/SignatureHandler.h"

void FSignatureService::SignCalls(const TFunction<void(TSharedPtr<FRawSignature>)>& OnSuccess, const TFunction<void(FString)>& OnFailure)
{
	SignSapient([this, OnSuccess, OnFailure](const TSharedPtr<FSignatureOfSapientSignerLeaf>& Signature)
	{
		Envelope->Sign(Signature);
		
		UGuardSigner* Guard = NewObject<UGuardSigner>();
		Guard->WithHost(Signers[0].Credentials.Guard.Url);

		Guard->SignEnvelope(Envelope, ConfigUpdates, ImageHash, [this, OnSuccess, OnFailure](const TSharedPtr<FEnvelope>& EnvelopeRef, const TArray<FConfigUpdate>& Updates, const TSharedPtr<FSignatureOfSignerLeafHash>& Signature)
		{
			if (!EnvelopeRef.IsValid() || !Signature.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("Envelope and/or Signature are nullptrs"));
				OnFailure(TEXT("Envelope and/or Signature are nullptrs"));
				return;
			}
			
			UE_LOG(LogTemp, Display, TEXT("Got RSY from Guard, %s"), *FByteArrayUtils::BytesToHexString(Signature.Get()->Signature->Pack()));
			EnvelopeRef->Signatures.Add(Signature);

			TSharedPtr<FRawSignature> RawSignature = FSignatureHandler::EncodeSignature(EnvelopeRef, ImageHash);
			if (RawSignature == nullptr || !RawSignature.IsValid())
			{
				UE_LOG(LogTemp, Error, TEXT("RawSignature is null"));
				OnFailure(TEXT("RawSignature is null"));
				return;
			}

			TArray<TSharedPtr<FRawSignature>> SignaturesFromConfigUpdates;
			for (int i = Updates.Num() - 1; i >= 0; i--)
			{
				const TSharedPtr<FRawSignature> Sig = FRawSignature::Decode(FByteArrayUtils::HexStringToBytes(Updates[i].Signature));
				SignaturesFromConfigUpdates.Add(Sig);
			}
			
			RawSignature->Suffix = SignaturesFromConfigUpdates;
			OnSuccess(RawSignature);
		}, OnFailure);
	}, OnFailure);
}

void FSignatureService::SignSapient(const TFunction<void(TSharedPtr<FSignatureOfSapientSignerLeaf>)>& OnSuccess, const TFunction<void(FString)>& OnFailure)
{
	const FCalls* CallsPayload = Envelope.Get()->Payload.Get();
	TArray<FCall> Calls = CallsPayload->Calls;
	FBigInt Space = CallsPayload->Space;
	FBigInt Nonce = CallsPayload->Nonce;
	
	FSignerService SignerService = FSignerService(ChainId, Calls, Signers, SessionsTopology);
	
	SignerService.FindSignersForCalls([this, Calls, Space, Nonce, OnSuccess](TArray<FSessionSigner> SupportedSigners)
	{
		
		TArray<FString> ImplicitSigners;
		TArray<FString> ExplicitSigners;

		TArray<TSharedPtr<FSessionCallSignature>> Signatures;
		for (int i = 0; i < SupportedSigners.Num(); i++)
		{
			FSessionSigner Signer = SupportedSigners[i];
			TSharedPtr<FSessionCallSignature> CallSignature = Signer.SignCall(ChainId, Calls[i], i, SessionsTopology, Space, Nonce);
			Signatures.Add(CallSignature);
			
			if (Signer.Credentials.IsExplicit)
			{
				ExplicitSigners.Add(Signer.GetSessionAddress());
			}
			else
			{
				ImplicitSigners.Add(Signer.GetSessionAddress());
			}
		}

		const TArray<uint8> Data = FSessionCallSignature::EncodeSignatures(Signatures, SessionsTopology, ImplicitSigners, ExplicitSigners);
		
		const FString SessionsAddress = FExtensionsFactory::GetCurrent().Sessions;
		const TSharedPtr<FSignatureOfSapientSignerLeaf> Signature = MakeShared<FSignatureOfSapientSignerLeaf>(FSignatureOfSapientSignerLeaf(ESapientSignatureType::SapientSignature, SessionsAddress, Data));
		
		OnSuccess(Signature);
	}, OnFailure);
}
