#include "EcosystemWallet/SequenceEcosystemWallet.h"

#include "Requests/SendTransactionArgs.h"
#include "Requests/SendTransactionResponse.h"
#include "Requests/SignMessageArgs.h"
#include "Requests/SignMessageResponse.h"
#include "Sequence/SequenceSdk.h"
#include "Storage/SessionStorage.h"

USequenceEcosystemWallet::USequenceEcosystemWallet()
{
	this->Client = NewObject<UEcosystemClient>();
	this->SessionStorage = NewObject<USessionStorage>();
}

void USequenceEcosystemWallet::AddSession(const TScriptInterface<IPermissions>& Permissions, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure)
{
	FSessionPermissions SessionPermissions = Permissions.GetInterface()->GetSessionPermissions();
	this->Client->CreateNewSession(ESessionCreationType::AddExplicit, "",
		"", SessionPermissions, OnSuccess, OnFailure);
}

void USequenceEcosystemWallet::SignMessage(const FString& Message, TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure)
{
	FSignMessageArgs Payload;
	Payload.ChainId = SequenceSdk::GetChainId();
	Payload.Address = this->GetWalletInfo().Address;
	Payload.Message = Message;

	const TSuccessCallback<FSignMessageResponse> OnClientSuccess = [OnSuccess](const FSignMessageResponse& Response)
	{
		OnSuccess(Response.Signature);
	};
	
	this->Client->SendRequest<FSignMessageArgs, FSignMessageResponse>("sign", "signMessage", Payload, OnClientSuccess, OnFailure);	
}

void USequenceEcosystemWallet::SendTransaction(TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure)
{
	// TODO
	OnFailure(FSequenceError(EErrorType::EmptyResponse, TEXT("")));
}

void USequenceEcosystemWallet::SendTransactionThroughEcosystem(TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure)
{
	FTransactionRequest Request;
	Request.To = "";
	Request.Data = "";
	Request.Value = 0;
	Request.GasLimit = 0;
	
	FSendTransactionArgs Payload;
	Payload.ChainId = SequenceSdk::GetChainId();
	Payload.Address = this->GetWalletInfo().Address;
	Payload.TransactionRequest = Request;

	const TSuccessCallback<FSendTransactionResponse> OnClientSuccess = [OnSuccess](const FSendTransactionResponse& Response)
	{
		OnSuccess(Response.TransactionHash);
	};
	
	this->Client->SendRequest<FSendTransactionArgs, FSendTransactionResponse>("sign", "signMessage", Payload, OnClientSuccess, OnFailure);	
}

void USequenceEcosystemWallet::ClearSessions()
{
	this->SessionStorage->ClearSessions();
}

FWalletInfo USequenceEcosystemWallet::GetWalletInfo()
{
	const FWalletSessions WalletSessions = this->SessionStorage->GetStoredSessions();
	if (!WalletSessions.HasSessions())
	{
		return FWalletInfo();
	}

	FSessionCredentials Session = WalletSessions.Sessions[0];
	
	FWalletInfo Info;
	Info.Address = Session.Address;
	Info.Email = Session.Email;

	return Info;
}

bool USequenceEcosystemWallet::CheckIfWalletExists()
{
	const FWalletSessions WalletSessions = this->SessionStorage->GetStoredSessions();
	return WalletSessions.HasSessions();	
}
