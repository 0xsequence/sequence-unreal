#pragma once

#include "Util/Async.h"
#include "SequenceConnect.generated.h"

class UEcosystemClient;

UCLASS()
class SEQUENCEPLUGIN_API USequenceConnect : public UObject
{
	GENERATED_BODY()
	
public:
	USequenceConnect();
	
	void SignInWithGoogle(TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure);

private:
	UPROPERTY()
	UEcosystemClient* Client;
};
