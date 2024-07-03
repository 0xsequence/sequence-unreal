// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "TransakDataTypes.h"
#include "Transak.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UTransakOnRamp : public UObject
{
	GENERATED_BODY()
private:
	FString WalletAddress = "";
public:
	UTransakOnRamp();
	UTransakOnRamp(const FString& WalletAddressIn);
	static void GetSupportedCountries(TSuccessCallback<TArray<FSupportedCountry>> OnSuccess, FFailureCallback OnFailure);
	void OpenTransakLink(const FString& FiatCurrencyIn = FDefaultTransak::FiatCurrency, const FString& DefaultFiatAmountIn = FDefaultTransak::FiatAmount, const FString& DefaultCryptoCurrencyIn = FDefaultTransak::CryptoCurrency, const FString& NetworksIn = FDefaultTransak::Networks, bool DisableWalletAddressFormIn = FDefaultTransak::DisableWalletAddressForm);
private:
	FString GetTransakLink(const FString& FiatCurrencyIn = FDefaultTransak::FiatCurrency, const FString& DefaultFiatAmountIn = FDefaultTransak::FiatAmount, const FString& DefaultCryptoCurrencyIn = FDefaultTransak::CryptoCurrency, const FString& NetworksIn = FDefaultTransak::Networks, bool DisableWalletAddressFormIn = FDefaultTransak::DisableWalletAddressForm);
};