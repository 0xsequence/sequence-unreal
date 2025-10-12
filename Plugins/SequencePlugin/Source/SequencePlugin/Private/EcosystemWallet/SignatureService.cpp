#include "SignatureService.h"
#include "SignerService.h"

void FSignatureService::SignCalls(TFunction<void(FRawSignature)> OnSuccess, TFunction<void()> OnFailure)
{
	SignSapient([](FSignatureOfSapientSignerLeaf Signature)
	{
		UE_LOG(LogTemp, Display, TEXT("Got sapient signature"));
	}, OnFailure);
}

void FSignatureService::SignSapient(TFunction<void(FSignatureOfSapientSignerLeaf)> OnSuccess,
	TFunction<void()> OnFailure)
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

		TArray<FSessionCallSignature> Signatures;
		for (int i = 0; i < SupportedSigners.Num(); i++)
		{
			FSessionSigner Signer = SupportedSigners[i];
			FSessionCallSignature CallSignature = Signer.SignCall(ChainId, Calls[i], i, SessionsTopology, Space, Nonce);
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
		const FSignatureOfSapientSignerLeaf Signature = FSignatureOfSapientSignerLeaf(SessionsAddress, Data);
		
		OnSuccess(Signature);
	}, OnFailure);
}
