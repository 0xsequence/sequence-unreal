#include "EcosystemWallet/SequenceWallet.h"

#include "TransactionService.h"
#include "WalletState.h"
#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "Requests/SendTransactionArgs.h"
#include "Requests/SendTransactionResponse.h"
#include "Requests/SignMessageArgs.h"
#include "Requests/SignMessageResponse.h"
#include "Sequence/SequenceSdk.h"
#include "Storage/SessionStorage.h"

USequenceWallet::USequenceWallet()
{
	this->Client = NewObject<UEcosystemClient>();
	this->WalletState = MakeShared<FWalletState>(FWalletState());
}

void USequenceWallet::AddSession(const TScriptInterface<IPermissions>& Permissions, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure)
{
	this->Client->CreateNewSession(ESessionCreationType::AddExplicit, "",
		"", Permissions, OnSuccess, OnFailure);
}

void USequenceWallet::SignMessage(const FString& Message, TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure)
{
	FSignMessageArgs Payload;
	Payload.ChainId = SequenceSdk::GetChainId();
	Payload.Address = this->GetWalletInfo().Address;
	Payload.Message = Message;

	const TFunction<void(FSignMessageResponse)> OnClientSuccess = [OnSuccess](const FSignMessageResponse& Response)
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

	const TFunction<void(FString)> OnInternalFailure = [OnFailure](const FString& Error)
	{
		OnFailure(FSequenceError(EErrorType::EmptyResponse, Error));
	};
	
	this->WalletState->UpdateState(this->GetWalletInfo().Address, [this, Transaction, OnInternalFailure]()
	{
		const FCalls Calls = Transaction.GetInterface()->GetCalls();

		const TArray<FSessionSigner> Signers = FSessionStorage::GetStoredSigners();

		FBigInt ChainId = FBigInt(SequenceSdk::GetChainIdString());
		FTransactionService TransactionService = FTransactionService(Signers, this->WalletState);
		TransactionService.SignAndBuild(ChainId, Calls.Calls, true, [](TTuple<FString, TArray<uint8>> Result)
		{
				
		}, OnInternalFailure);
	}, OnInternalFailure);
}

void USequenceWallet::SendTransactionWithoutPermissions(const TScriptInterface<ISeqTransactionBase>& Transaction, TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure)
{
	FCall Call = Transaction.GetInterface()->GetCalls().Calls[0];
	
	FTransactionRequest Request;
	Request.To = Call.To;
	Request.Data = Call.Data;
	Request.Value = Call.Value.Value;
	Request.GasLimit = Call.GasLimit.Value;
	
	FSendTransactionArgs Payload;
	Payload.ChainId = SequenceSdk::GetChainId();
	Payload.Address = this->GetWalletInfo().Address;
	Payload.TransactionRequest = Request;

	const TFunction<void(FSendTransactionResponse)> OnClientSuccess = [OnSuccess](const FSendTransactionResponse& Response)
	{
		OnSuccess(Response.TransactionHash);
	};
	
	this->Client->SendRequest<FSendTransactionArgs, FSendTransactionResponse>("transaction", "sendWalletTransaction", Payload, OnClientSuccess, OnFailure);	
}

void USequenceWallet::ClearSessions()
{
	FSessionStorage::ClearSessions();
}

FWalletInfo USequenceWallet::GetWalletInfo()
{
	const FWalletSessions WalletSessions = FSessionStorage::GetStoredSessions();
	if (!WalletSessions.HasSessions())
	{
		return FWalletInfo();
	}

	FSessionCredentials Session = WalletSessions.Sessions[0];
	
	FWalletInfo Info;
	Info.Address = Session.WalletAddress;
	Info.Email = Session.UserEmail;

	return Info;
}

bool USequenceWallet::CheckIfWalletExists()
{
	const FWalletSessions WalletSessions = FSessionStorage::GetStoredSessions();
	return WalletSessions.HasSessions();	
}
