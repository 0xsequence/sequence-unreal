// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Subsystems/SequencePayBP.h"
#include "Transak.h"

FString USequencePayBP::GetOnRampLink(const FString& WalletAddress, const FTransakSettings& Settings)
{
	UTransakOnRamp* OnRamp = UTransakOnRamp::Init(WalletAddress);
	return OnRamp->GetTransakLink(Settings.FiatCurrency, Settings.FiatAmount, Settings.CryptoCurrency, Settings.Networks, Settings.DisableWalletAddressForm);
}

void USequencePayBP::OpenOnRampInExternalBrowser(const FString& WalletAddress, const FTransakSettings& Settings)
{
	UTransakOnRamp* OnRamp = UTransakOnRamp::Init(WalletAddress);
	OnRamp->OpenTransakLink(Settings.FiatCurrency, Settings.FiatAmount, Settings.CryptoCurrency, Settings.Networks, Settings.DisableWalletAddressForm);
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
	
	const UTransakOnRamp* OnRamp = NewObject<UTransakOnRamp>();
	OnRamp->GetSupportedCountries(SuccessCallback,FailureCallback);
}
