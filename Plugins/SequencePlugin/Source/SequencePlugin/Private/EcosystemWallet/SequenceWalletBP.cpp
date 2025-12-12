#include "EcosystemWallet/SequenceWalletBP.h"
#include "Util/Async.h"
#include "Util/Log.h"

USequenceWalletBP::USequenceWalletBP()
{
	this->Wallet = NewObject<USequenceWallet>();
}

void USequenceWalletBP::AddSession(const TScriptInterface<IPermissions>& Permissions, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<bool> SuccessCallback = [OnSuccess](const bool& Result)
	{
		OnSuccess.ExecuteIfBound();
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->GetSequenceEcosystemWallet()->AddSession(Permissions, SuccessCallback, FailureCallback);	
}

void USequenceWalletBP::SignMessage(const FString& Message, FOnSignature OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FString> SuccessCallback = [OnSuccess](const FString& Signature)
	{
		OnSuccess.ExecuteIfBound(Signature);
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->GetSequenceEcosystemWallet()->SignMessage(Message, SuccessCallback, FailureCallback);	
}

void USequenceWalletBP::GetFeeOptions(const TScriptInterface<ISeqTransactionBase>& Transaction, FOnFeeOptions OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FFeeOptionsResponse> SuccessCallback = [OnSuccess](const FFeeOptionsResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->GetSequenceEcosystemWallet()->GetFeeOptions(Transaction, SuccessCallback, FailureCallback);
}

void USequenceWalletBP::SendTransaction(const TScriptInterface<ISeqTransactionBase>& Transaction, const FFeeOption FeeOption, FOnTransaction OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FString> SuccessCallback = [OnSuccess](const FString& Signature)
	{
		OnSuccess.ExecuteIfBound(Signature);
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->GetSequenceEcosystemWallet()->SendTransaction(Transaction, FeeOption, SuccessCallback, FailureCallback);
}

void USequenceWalletBP::SendTransactionWithoutPermissions(const TScriptInterface<ISeqTransactionBase>& Transaction, FOnTransaction OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FString> SuccessCallback = [OnSuccess](const FString& Hash)
	{
		OnSuccess.ExecuteIfBound(Hash);
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->GetSequenceEcosystemWallet()->SendTransactionWithoutPermissions(Transaction, SuccessCallback, FailureCallback);	
}

void USequenceWalletBP::ClearSessions()
{
	this->GetSequenceEcosystemWallet()->ClearSessions();
	this->CallOnSessionClosed();
}

FWalletInfo USequenceWalletBP::GetWalletInfo()
{
	return this->GetSequenceEcosystemWallet()->GetWalletInfo();
}

bool USequenceWalletBP::CheckIfWalletExists()
{
	return this->GetSequenceEcosystemWallet()->CheckIfWalletExists();	
}

USequenceWallet* USequenceWalletBP::GetSequenceEcosystemWallet() const
{
	return this->Wallet;
}

void USequenceWalletBP::CallOnSessionClosed() const
{
	if (this->OnSessionClosed.IsBound())
		this->OnSessionClosed.Broadcast();
	else
		SEQ_LOG(Warning, TEXT("Session was created but no event listener was found for OnSessionClosed"));
}
