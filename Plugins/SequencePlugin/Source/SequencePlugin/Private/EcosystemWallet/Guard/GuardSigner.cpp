#include "GuardSigner.h"

void UGuardSigner::WithHost(const FString& Host)
{
	this->GuardService = NewObject<UGuardService>();
	this->GuardService->WithHost(Host);
}

void UGuardSigner::SignEnvelope()
{
	
}

void UGuardSigner::SignPayload(const FString& Wallet, const FString& ChainId, const TArray<uint8>& Digest,
	const TArray<uint8>& Message, const TArray<FGuardSignatureArgs>& Signatures,
	TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
}
