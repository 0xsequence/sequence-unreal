#include "EcosystemWallet/SequenceConnect.h"
#include "EcosystemClient.h"
#include "Util/HttpHandler.h"

USequenceConnect::USequenceConnect()
{
	this->Client = NewObject<UEcosystemClient>();
}

void USequenceConnect::GetEcosystemConfig(const TSuccessCallback<FEcosystemConfig>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FString WalletAppUrl = UConfigFetcher::GetConfigVar("WalletAppUrl");
	UHttpHandler* HttpHandler = NewObject<UHttpHandler>();
	HttpHandler->SetRequestUrl(WalletAppUrl);
	HttpHandler->SendGetRequest("api/wallet-configuration", [OnSuccess](const FString& Response)
	{
		const FEcosystemConfig Config = USequenceSupport::JSONStringToStruct<FEcosystemConfig>(Response);
		OnSuccess(Config);
	},
	[OnFailure](const FString& Response)
	{
		OnFailure(FSequenceError(EErrorType::RequestFail, Response));
	});
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
