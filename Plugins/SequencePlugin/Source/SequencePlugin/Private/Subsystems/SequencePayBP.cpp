// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Subsystems/SequencePayBP.h"

USequencePayBP::USequencePayBP()
{
	this->Pay = NewObject<USequencePay>();
}

FString USequencePayBP::GetOnRampLink(const FString& WalletAddress, const FTransakSettings& Settings)
{
	return this->Pay->GetOnRampLink(WalletAddress, Settings.FiatCurrency, Settings.FiatAmount, Settings.CryptoCurrency, Settings.Networks, Settings.DisableWalletAddressForm);
}

void USequencePayBP::OpenOnRampInExternalBrowser(const FString& WalletAddress, const FTransakSettings& Settings)
{
	this->Pay->OpenOnRampInExternalBrowser(WalletAddress, Settings.FiatCurrency, Settings.FiatAmount, Settings.CryptoCurrency, Settings.Networks, Settings.DisableWalletAddressForm);
}

void USequencePayBP::GetSupportedOnRampCountries(FOnSupportedTransakCountries OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<TArray<FSupportedCountry>> SuccessCallback = [OnSuccess](const TArray<FSupportedCountry>& Countries)
	{
		OnSuccess.ExecuteIfBound(Countries);
	};
	
	const FFailureCallback FailureCallback = [OnFailure](const FSequenceError& Error)
	{
		OnFailure.ExecuteIfBound(Error.Message);	
	};
	
	this->Pay->GetSupportedOnRampCountries(SuccessCallback,FailureCallback);
}
