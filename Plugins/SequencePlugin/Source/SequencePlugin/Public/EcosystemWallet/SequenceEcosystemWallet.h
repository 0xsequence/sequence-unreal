#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "SequenceEcosystemWallet.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API USequenceEcosystemWallet : public UObject
{
	GENERATED_BODY()

public:
	void SignMessage(const FString& Message, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure);
};
