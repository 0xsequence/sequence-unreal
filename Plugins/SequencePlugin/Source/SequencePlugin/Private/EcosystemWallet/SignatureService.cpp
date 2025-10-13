#include "SignatureService.h"
#include "SignerService.h"
#include "Guard/GuardSigner.h"

void FSignatureService::SignCalls(const TFunction<void(FRawSignature)>& OnSuccess, const TFunction<void(FString)>& OnFailure)
{
	SignSapient([this, OnFailure](const TSharedPtr<FSignatureOfSapientSignerLeaf>& Signature)
	{
		UE_LOG(LogTemp, Display, TEXT("Got sapient signature, %s"), *FByteArrayUtils::BytesToHexString(Signature.Get()->Data));

		Envelope->Sign(Signature);
		
		UGuardSigner* Guard = NewObject<UGuardSigner>();
		Guard->WithHost(Signers[0].Credentials.Guard.Url);
		
		Guard->SignEnvelope(Envelope, ImageHash, [](const TSharedPtr<FRSY>& RSY)
		{
			UE_LOG(LogTemp, Display, TEXT("Got RSY from Guard, %s"), *FByteArrayUtils::BytesToHexString(RSY.Get()->Pack()));
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
				ExplicitSigners.Add(Signer.Credentials.SessionAddress);
			}
			else
			{
				ImplicitSigners.Add(Signer.Credentials.SessionAddress);
			}
		}

		const TArray<uint8> Data = FSessionCallSignature::EncodeSignatures(Signatures, SessionsTopology, ImplicitSigners, ExplicitSigners);
		
		const FString SessionsAddress = "0x0000000000CC58810c33F3a0D78aA1Ed80FaDcD8";
		const TSharedPtr<FSignatureOfSapientSignerLeaf> Signature = MakeShared<FSignatureOfSapientSignerLeaf>(FSignatureOfSapientSignerLeaf(SessionsAddress, Data));
		
		OnSuccess(Signature);
	}, OnFailure);
}
