#pragma once

#include "CoreMinimal.h"
#include "GuardService.h"
#include "UObject/Object.h"
#include "GuardSigner.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UGuardSigner : public UObject
{
	GENERATED_BODY()

public:
	void WithHost(const FString& Host);

	void SignEnvelope();

private:
	
	void SignPayload(const FString& Wallet, const FString& ChainId, const TArray<uint8>& Digest,
		const TArray<uint8>& Message, const TArray<FGuardSignatureArgs>& Signatures,
		TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);
	
	UPROPERTY()
	UGuardService* GuardService;
};
