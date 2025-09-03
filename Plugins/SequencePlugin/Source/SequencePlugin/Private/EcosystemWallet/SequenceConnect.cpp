#include "EcosystemWallet/SequenceConnect.h"
#include "EcosystemClient.h"

USequenceConnect::USequenceConnect()
{
	this->Client = NewObject<UEcosystemClient>();
}

void USequenceConnect::SignInWithEmail(const FString& Email, const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	FSessionPermissions SessionPermissions = Permissions.GetInterface()->GetSessionPermissions();
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "email",
		Email, SessionPermissions, OnSuccess, OnFailure);
}

void USequenceConnect::SignInWithGoogle(const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	FSessionPermissions SessionPermissions = Permissions.GetInterface()->GetSessionPermissions();
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "google",
		"", SessionPermissions, OnSuccess, OnFailure);
}

void USequenceConnect::SignInWithApple(const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	FSessionPermissions SessionPermissions = Permissions.GetInterface()->GetSessionPermissions();
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "apple",
	"", SessionPermissions, OnSuccess, OnFailure);
}

void USequenceConnect::SignInWithPasskey(const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	FSessionPermissions SessionPermissions = Permissions.GetInterface()->GetSessionPermissions();
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "passkey",
	"", SessionPermissions, OnSuccess, OnFailure);
}

void USequenceConnect::SignInWithMnemonic(const TScriptInterface<IPermissions>& Permissions, const TSuccessCallback<bool>& OnSuccess, const FFailureCallback& OnFailure)
{
	FSessionPermissions SessionPermissions = Permissions.GetInterface()->GetSessionPermissions();
	this->Client->CreateNewSession(ESessionCreationType::CreateNewSession, "mnemonic",
	"", SessionPermissions, OnSuccess, OnFailure);
}
