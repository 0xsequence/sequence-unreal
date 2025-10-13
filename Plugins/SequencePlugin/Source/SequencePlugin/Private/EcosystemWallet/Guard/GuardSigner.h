#pragma once

#include "CoreMinimal.h"
#include "GuardService.h"
#include "EcosystemWallet/Primitives/Envelope/Envelope.h"
#include "EcosystemWallet/Primitives/Signatures/RSY.h"
#include "EcosystemWallet/Primitives/Signatures/SignatureOfSignerLeafHash.h"
#include "UObject/Object.h"
#include "GuardSigner.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UGuardSigner : public UObject
{
	GENERATED_BODY()

public:
	void WithHost(const FString& Host);

	void SignEnvelope(
		const TSharedPtr<FEnvelope>& Envelope,
		const FString& SessionsImageHash,
		const TFunction<void(TSharedPtr<FEnvelope>, TSharedPtr<FSignatureOfSignerLeafHash>)>& OnSuccess,
		const TFunction<void(FString)>& OnFailure);

private:
	UPROPERTY()
	UGuardService* GuardService;
};
