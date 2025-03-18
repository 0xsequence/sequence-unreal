// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TransakDataTypes.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "SequencePay.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API USequencePay : public UObject
{
	GENERATED_BODY()

public:
	FString GetOnRampLink(const FString& WalletAddress, const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn);
	
	void OpenOnRampInExternalBrowser(const FString& WalletAddress, const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn);

	void GetSupportedOnRampCountries(TSuccessCallback<TArray<FSupportedCountry>> OnSuccess, FFailureCallback OnFailure);
};
