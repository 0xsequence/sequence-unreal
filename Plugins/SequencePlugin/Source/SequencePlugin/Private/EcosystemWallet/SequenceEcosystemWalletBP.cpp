#include "EcosystemWallet/SequenceEcosystemWalletBP.h"
#include "Util/Async.h"
#include "Util/Log.h"

USequenceEcosystemWalletBP::USequenceEcosystemWalletBP()
{
	this->Wallet = NewObject<USequenceEcosystemWallet>();
}

void USequenceEcosystemWalletBP::AddSession(const TScriptInterface<IPermissions>& Permissions, FOnSuccess OnSuccess, FOnFailure OnFailure)
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

void USequenceEcosystemWalletBP::SignMessage(const FString& Message, FOnSignature OnSuccess, FOnFailure OnFailure)
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

void USequenceEcosystemWalletBP::SendTransaction(const TScriptInterface<ISeqTransactionBase>& Transaction, FOnTransaction OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FString> SuccessCallback = [OnSuccess](const FString& Signature)
	{
		OnSuccess.ExecuteIfBound(Signature);
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->GetSequenceEcosystemWallet()->SendTransaction(Transaction, SuccessCallback, FailureCallback);
}

void USequenceEcosystemWalletBP::SendTransactionWithoutPermissions(const TScriptInterface<ISeqTransactionBase>& Transaction, FOnTransaction OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FString> SuccessCallback = [OnSuccess](const FString& Signature)
	{
		OnSuccess.ExecuteIfBound(Signature);
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->GetSequenceEcosystemWallet()->SendTransactionWithoutPermissions(Transaction, SuccessCallback, FailureCallback);	
}

void USequenceEcosystemWalletBP::ClearSessions()
{
	this->GetSequenceEcosystemWallet()->ClearSessions();
	this->CallOnSessionClosed();
}

FWalletInfo USequenceEcosystemWalletBP::GetWalletInfo()
{
	return this->GetSequenceEcosystemWallet()->GetWalletInfo();
}

bool USequenceEcosystemWalletBP::CheckIfWalletExists()
{
	return this->GetSequenceEcosystemWallet()->CheckIfWalletExists();	
}

USequenceEcosystemWallet* USequenceEcosystemWalletBP::GetSequenceEcosystemWallet() const
{
	return this->Wallet;
}

void USequenceEcosystemWalletBP::CallOnSessionClosed() const
{
	if (this->OnSessionClosed.IsBound())
		this->OnSessionClosed.Broadcast();
	else
		SEQ_LOG(Warning, TEXT("Session was created but no event listener was found for OnSessionClosed"));
}
