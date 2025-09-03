#include "EcosystemWallet/SequenceEcosystemWallet.h"
#include "Storage/SessionStorage.h"

USequenceEcosystemWallet::USequenceEcosystemWallet()
{
	this->SessionStorage = NewObject<USessionStorage>();
}


void USequenceEcosystemWallet::SignMessage(const FString& Message, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure)
{
	
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
