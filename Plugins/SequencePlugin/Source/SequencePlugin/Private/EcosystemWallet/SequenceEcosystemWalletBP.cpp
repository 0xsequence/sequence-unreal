#include "EcosystemWallet/SequenceEcosystemWalletBP.h"
#include "Util/Async.h"

USequenceEcosystemWalletBP::USequenceEcosystemWalletBP()
{
	this->Wallet = NewObject<USequenceEcosystemWallet>();
}

void USequenceEcosystemWalletBP::SignMessage(const FString& Message, FOnSuccess OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<bool> SuccessCallback = [OnSuccess](bool Result)
	{
		OnSuccess.ExecuteIfBound();
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->GetSequenceEcosystemWallet()->SignMessage(Message, SuccessCallback, FailureCallback);	
}

bool USequenceEcosystemWalletBP::CheckIfWalletExists()
{
	return this->GetSequenceEcosystemWallet()->CheckIfWalletExists();	
}

USequenceEcosystemWallet* USequenceEcosystemWalletBP::GetSequenceEcosystemWallet() const
{
	return this->Wallet;
}
