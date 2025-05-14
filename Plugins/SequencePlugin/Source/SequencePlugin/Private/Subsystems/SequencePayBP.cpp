#include "Subsystems/SequencePayBP.h"
#include "Marketplace/Structs/SeqGetSwapPriceResponse.h"
#include "Marketplace/Structs/SeqGetSwapQuoteResponse.h"
#include "Sequence/SequenceSdk.h"
#include "Util/Log.h"

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

void USequencePayBP::GetSwapPrice(const FString& SellCurrency, const FString& BuyCurrency, const FString& BuyAmount, const int SlippagePercentage, FOnGetSwapPrice OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqSwapPrice> OnApiSuccess = [this, OnSuccess](const FSeqSwapPrice& Response)
	{
		OnSuccess.ExecuteIfBound(Response.CurrencyAddress, Response.CurrencyBalance, Response.Price, Response.MaxPrice, Response.TransactionValue);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Swap Price: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Pay->GetSwapPrice(SequenceSdk::GetChainId(), SellCurrency, BuyCurrency, BuyAmount, OnApiSuccess, OnApiFailure, SlippagePercentage);
}

void USequencePayBP::GetSwapPrices(const FString& UserWallet, const FString& BuyCurrency, const FString& BuyAmount, const int SlippagePercentage, FOnGetSwapPrices OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<TArray<FSeqSwapPrice>> OnApiSuccess = [this, OnSuccess](const TArray<FSeqSwapPrice>& SwapPrices)
	{
		OnSuccess.ExecuteIfBound(SwapPrices);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Swap Prices: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Pay->GetSwapPrices(SequenceSdk::GetChainId(), UserWallet, BuyCurrency, BuyAmount, OnApiSuccess, OnApiFailure, SlippagePercentage);
}

void USequencePayBP::GetSwapQuote(const FString& UserWallet, const FString& BuyCurrency, const FString& SellCurrency, const FString& BuyAmount, const bool IncludeApprove, const int SlippagePercentage, FOnGetSwapQuote OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqSwapQuote> OnApiSuccess = [this, OnSuccess](const FSeqSwapQuote& Response)
	{
		OnSuccess.ExecuteIfBound(Response.CurrencyAddress, Response.CurrencyBalance, Response.Price, Response.MaxPrice, Response.To, Response.TransactionData, Response.TransactionValue, Response.ApproveData);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Swap Quote: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Pay->GetSwapQuote(SequenceSdk::GetChainId(), UserWallet, BuyCurrency, SellCurrency, BuyAmount, IncludeApprove, OnApiSuccess, OnApiFailure, SlippagePercentage);
}

USequencePay* USequencePayBP::GetSequencePay() const
{
	return this->Pay;
}
