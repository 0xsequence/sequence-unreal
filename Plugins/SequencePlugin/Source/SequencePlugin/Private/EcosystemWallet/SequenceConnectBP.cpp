#include "EcosystemWallet/SequenceConnectBP.h"

#include "Util/Async.h"

USequenceConnectBP::USequenceConnectBP()
{
	this->SequenceConnect = NewObject<USequenceConnect>();
}

void USequenceConnectBP::SignInWithGoogle(FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<bool> SuccessCallback = [OnSuccess](bool Result)
	{
		OnSuccess.ExecuteIfBound();
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->SequenceConnect->SignInWithGoogle(SuccessCallback, FailureCallback);
}

USequenceConnect* USequenceConnectBP::GetSequenceConnect() const
{
	return this->SequenceConnect;
}

