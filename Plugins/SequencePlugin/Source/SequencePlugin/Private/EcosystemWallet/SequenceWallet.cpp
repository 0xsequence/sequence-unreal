#include "EcosystemWallet/SequenceWallet.h"
#include "WalletState.h"
#include "Primitives/Calls/Call.h"
#include "Requests/SendTransactionArgs.h"
#include "Requests/SendTransactionResponse.h"
#include "Requests/SignMessageArgs.h"
#include "Requests/SignMessageResponse.h"
#include "Sequence/SequenceSdk.h"
#include "Storage/SessionStorage.h"

USequenceWallet::USequenceWallet()
{
	this->Client = NewObject<UEcosystemClient>();
	this->SessionStorage = NewObject<USessionStorage>();
	this->WalletState = NewObject<UWalletState>();
}

void USequenceWallet::AddSession(const TScriptInterface<IPermissions>& Permissions, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure)
{
	FSessionPermissions SessionPermissions = Permissions.GetInterface()->GetSessionPermissions();
	this->Client->CreateNewSession(ESessionCreationType::AddExplicit, "",
		"", SessionPermissions, OnSuccess, OnFailure);
}

void USequenceWallet::SignMessage(const FString& Message, TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure)
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

void USequenceWallet::SendTransaction(const TScriptInterface<ISeqTransactionBase>& Transaction, TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure)
{
	const TFunction<void()> OnWalletStateUpdated = [OnFailure]()
	{
		OnFailure(FSequenceError(EErrorType::EmptyResponse, TEXT("")));
	};

	const TFunction<void()> OnWalletStateFailed = [OnFailure]()
	{
		OnFailure(FSequenceError(EErrorType::EmptyResponse, TEXT("")));
	};
	
	this->WalletState->UpdateState(this->GetWalletInfo().Address, OnWalletStateUpdated, OnWalletStateFailed);
}

void USequenceWallet::SendTransactionWithoutPermissions(const TScriptInterface<ISeqTransactionBase>& Transaction, TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure)
{
	FCall Call = Transaction.GetInterface()->GetCalls().Calls[0];
	
	FTransactionRequest Request;
	Request.To = Call.To;
	Request.Data = Call.Data;
	Request.Value = Call.Value;
	Request.GasLimit = Call.GasLimit;
	
	FSendTransactionArgs Payload;
	Payload.ChainId = SequenceSdk::GetChainId();
	Payload.Address = this->GetWalletInfo().Address;
	Payload.TransactionRequest = Request;

	const TSuccessCallback<FSendTransactionResponse> OnClientSuccess = [OnSuccess](const FSendTransactionResponse& Response)
	{
		OnSuccess(Response.TransactionHash);
	};
	
	this->Client->SendRequest<FSendTransactionArgs, FSendTransactionResponse>("transaction", "sendWalletTransaction", Payload, OnClientSuccess, OnFailure);	
}

void USequenceWallet::ClearSessions()
{
	this->SessionStorage->ClearSessions();
}

FWalletInfo USequenceWallet::GetWalletInfo()
{
	const FWalletSessions WalletSessions = this->SessionStorage->GetStoredSessions();
	if (!WalletSessions.HasSessions())
	{
		return FWalletInfo();
	}

	FSessionCredentials Session = WalletSessions.Sessions[0];
	
	FWalletInfo Info;
	Info.Address = Session.Address;
	Info.Email = Session.UserEmail;

	return Info;
}

bool USequenceWallet::CheckIfWalletExists()
{
	const FWalletSessions WalletSessions = this->SessionStorage->GetStoredSessions();
	return WalletSessions.HasSessions();	
}
