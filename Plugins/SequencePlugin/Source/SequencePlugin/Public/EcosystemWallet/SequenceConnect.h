#pragma once

#include "Util/Async.h"
#include "CoreMinimal.h"
#include "SequenceConnect.generated.h"

class UEcosystemClient;

UCLASS()
class SEQUENCEPLUGIN_API USequenceConnect : public UObject
{
	GENERATED_BODY()
	
public:
	USequenceConnect();
	
	void SignInWithEmail(const FString& Email, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure);
	void SignInWithGoogle(const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure);
	void SignInWithApple(const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure);
	void SignInWithPasskey(const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure);
	void SignInWithMnemonic(const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure);

private:
	UPROPERTY()
	UEcosystemClient* Client;
};
