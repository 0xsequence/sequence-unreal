#include "EcosystemWallet/SequenceConnect.h"
#include "EcosystemClient.h"

USequenceConnect::USequenceConnect()
{
	this->Client = NewObject<UEcosystemClient>();
}

void USequenceConnect::SignInWithGoogle(const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "google",
		TOptional<FString>(), TOptional<FSessionPermissions>(), OnSuccess, OnFailure);
}
