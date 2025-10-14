#pragma once

#include "CoreMinimal.h"
#include "GuardService.h"
#include "EcosystemWallet/KeyMachine/Models/ConfigUpdatesResponse.h"
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
		const TArray<FConfigUpdate>& ConfigUpdates,
		const FString& SessionsImageHash,
		const TFunction<void(TSharedPtr<FEnvelope>, const TArray<FConfigUpdate>, TSharedPtr<FSignatureOfSignerLeafHash>)>& OnSuccess,
		const TFunction<void(FString)>& OnFailure);

private:
	UPROPERTY()
	UGuardService* GuardService;
};
