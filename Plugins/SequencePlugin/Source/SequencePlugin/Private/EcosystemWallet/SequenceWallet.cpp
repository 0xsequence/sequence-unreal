#include "EcosystemWallet/SequenceWallet.h"

#include "FeeOptionService.h"
#include "RelayerReceiptPoller.h"
#include "TransactionService.h"
#include "WalletState.h"
#include "EcosystemWallet/Primitives/Calls/Call.h"
#include "Relayer/SequenceRelayer.h"
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

void USequenceWallet::GetFeeOptions(const TScriptInterface<ISeqTransactionBase>& Transaction,
	TSuccessCallback<FFeeOptionsResponse> OnSuccess, const FFailureCallback& OnFailure)
{
	const TFunction<void(FString)> OnInternalFailure = [OnFailure](const FString& Error)
	{
		OnFailure(FSequenceError(EErrorType::EmptyResponse, Error));
	};
	
	this->WalletState->UpdateState(this->GetWalletInfo().Address, [this, Transaction, OnSuccess, OnInternalFailure]()
	{
		const FCalls Calls = Transaction.GetInterface()->GetCalls();

		const TArray<FSessionSigner> Signers = FSessionStorage::GetStoredSigners();

		const FBigInt ChainId = FBigInt(SequenceSdk::GetChainIdString());
		
		FTransactionService TransactionService = FTransactionService(Signers, this->WalletState);
		TransactionService.SignAndBuild(ChainId, Calls.Calls, true, [OnSuccess, OnInternalFailure](TTuple<FString, FString> Result)
		{
			const FString To = Result.Key;
			const FString Data = Result.Value;

			FFeeOptionsArgs Args;
			Args.Wallet = To;
			Args.To = To;
			Args.Data = Data;
			Args.Simulate = false;
			
			USequenceRelayer* Relayer = NewObject<USequenceRelayer>();
			Relayer->GetFeeOptions(Args, [OnSuccess](const FFeeOptionsResponse& Response)
			{
				OnSuccess(Response);
			}, OnInternalFailure);
		}, OnInternalFailure);
	}, OnInternalFailure);
}

void USequenceWallet::SendTransaction(const TScriptInterface<ISeqTransactionBase>& Transaction, const FFeeOption& FeeOption, TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure)
{
	FCalls Calls = Transaction.GetInterface()->GetCalls();
	if (Calls.Calls.Num() == 0)
	{
		OnFailure(FSequenceError(EErrorType::RequestFail, TEXT("Empty calls")));
		return;
	}
	
	const bool HasFeeOption = FeeOption.To != "" && FeeOption.Value != "";
	if (HasFeeOption)
	{
		FFeeOptionService FeeOptionService = FFeeOptionService(FeeOption);
		FCall FeeOptionCall = FeeOptionService.BuildCall();
		
		TArray<FCall> NewCalls = Calls.Calls;
		NewCalls.Insert(FeeOptionCall, 0);

		Calls.Calls = NewCalls;
	}
	
	const FString LogT = (HasFeeOption ? "true" : "false");
	UE_LOG(LogTemp, Display, TEXT("Has fee option %s"), *LogT);

	const TFunction<void(FString)> OnInternalFailure = [OnFailure](const FString& Error)
	{
		OnFailure(FSequenceError(EErrorType::RequestFail, Error));
	};
	
	this->WalletState->UpdateState(this->GetWalletInfo().Address, [this, Calls, OnSuccess, OnInternalFailure]()
	{
		const TArray<FSessionSigner> Signers = FSessionStorage::GetStoredSigners();

		const FBigInt ChainId = FBigInt(SequenceSdk::GetChainIdString());
		
		FTransactionService TransactionService = FTransactionService(Signers, this->WalletState);
		TransactionService.SignAndBuild(ChainId, Calls.Calls, true, [OnSuccess, OnInternalFailure](TTuple<FString, FString> Result)
		{
			const FString To = Result.Key;
			const FString Data = Result.Value;

			USequenceRelayer* Relayer = NewObject<USequenceRelayer>();
			Relayer->Relay(To, Data, "", TArray<FIntentPrecondition>(), [OnSuccess, OnInternalFailure](const FString& Receipt)
			{
				URelayerReceiptPoller* Poller = NewObject<URelayerReceiptPoller>();
				Poller->StartPolling(Receipt, [OnSuccess](const FString& TxnHash)
				{
					UE_LOG(LogTemp, Display, TEXT("Received transaction: %s"), *TxnHash);
					OnSuccess(TxnHash);
				}, OnInternalFailure);
			}, OnInternalFailure);
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
