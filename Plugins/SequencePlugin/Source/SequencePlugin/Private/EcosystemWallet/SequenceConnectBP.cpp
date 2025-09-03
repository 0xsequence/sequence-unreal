#include "EcosystemWallet/SequenceConnectBP.h"

#include "Util/Async.h"
#include "Util/Log.h"

USequenceConnectBP::USequenceConnectBP()
{
	this->SequenceConnect = NewObject<USequenceConnect>();
}

void USequenceConnectBP::SignInWithEmail(const FString& Email, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<bool> SuccessCallback = [this, OnSuccess](bool Result)
	{
		OnSuccess.ExecuteIfBound();
		this->CallOnSessionCreated();
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->SequenceConnect->SignInWithEmail(Email, SuccessCallback, FailureCallback);
}

void USequenceConnectBP::SignInWithGoogle(FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<bool> SuccessCallback = [this, OnSuccess](bool Result)
	{
		OnSuccess.ExecuteIfBound();
		this->CallOnSessionCreated();
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->SequenceConnect->SignInWithGoogle(SuccessCallback, FailureCallback);
}

void USequenceConnectBP::SignInWithApple(FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<bool> SuccessCallback = [this, OnSuccess](bool Result)
	{
		OnSuccess.ExecuteIfBound();
		this->CallOnSessionCreated();
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->SequenceConnect->SignInWithApple(SuccessCallback, FailureCallback);
}

void USequenceConnectBP::SignInWithPasskey(FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<bool> SuccessCallback = [this, OnSuccess](bool Result)
	{
		OnSuccess.ExecuteIfBound();
		this->CallOnSessionCreated();
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->SequenceConnect->SignInWithPasskey(SuccessCallback, FailureCallback);
}

void USequenceConnectBP::SignInWithMnemonic(FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<bool> SuccessCallback = [this, OnSuccess](bool Result)
	{
		OnSuccess.ExecuteIfBound();
		this->CallOnSessionCreated();
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->SequenceConnect->SignInWithMnemonic(SuccessCallback, FailureCallback);
}

USequenceConnect* USequenceConnectBP::GetSequenceConnect() const
{
	return this->SequenceConnect;
}

void USequenceConnectBP::CallOnSessionCreated() const
{
	if (this->OnSessionCreated.IsBound())
		this->OnSessionCreated.Broadcast();
	else
		SEQ_LOG(Warning, TEXT("Session was created but no event listener was found for OnSessionCreated"));
}
