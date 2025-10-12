#include "EcosystemWallet/SequenceConnect.h"
#include "EcosystemClient.h"

USequenceConnect::USequenceConnect()
{
	this->Client = NewObject<UEcosystemClient>();
}

void USequenceConnect::SignInWithEmail(const FString& Email, const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "email",
		Email, Permissions, OnSuccess, OnFailure);
}

void USequenceConnect::SignInWithGoogle(const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "google",
		"", Permissions, OnSuccess, OnFailure);
}

void USequenceConnect::SignInWithApple(const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "apple",
	"", Permissions, OnSuccess, OnFailure);
}

void USequenceConnect::SignInWithPasskey(const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "passkey",
	"", Permissions, OnSuccess, OnFailure);
}

void USequenceConnect::SignInWithMnemonic(const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "mnemonic",
	"", Permissions, OnSuccess, OnFailure);
}
