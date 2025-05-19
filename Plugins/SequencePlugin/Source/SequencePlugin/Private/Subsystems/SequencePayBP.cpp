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

void USequencePayBP::GetSupportedSwapChains(FOnGetSupportedSwapChains OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetLifiChainsResponse> OnApiSuccess = [this, OnSuccess](const FSeqGetLifiChainsResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response.ChainIds);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Swap Price: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Pay->GetSupportedSwapChains(OnApiSuccess, OnApiFailure);
}

void USequencePayBP::GetSupportedSwapTokens(const FSeqGetLifiTokensArgs& Args, FOnGetSupportedSwapTokens OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqGetLifiTokensResponse> OnApiSuccess = [this, OnSuccess](const FSeqGetLifiTokensResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response.Tokens);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Swap Price: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Pay->GetSupportedSwapTokens(Args, OnApiSuccess, OnApiFailure);
}

void USequencePayBP::GetSwapPrice(const FString& WalletAddress, const FString& SellCurrency, const FString& BuyCurrency, const FString& BuyAmount, FOnGetSwapPrice OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqSwapPrice> OnApiSuccess = [this, OnSuccess](const FSeqSwapPrice& Response)
	{
		OnSuccess.ExecuteIfBound(Response.CurrencyAddress, Response.Price);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Swap Price: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Pay->GetSwapPrice(SequenceSdk::GetChainId(), WalletAddress, SellCurrency, BuyCurrency, BuyAmount, OnApiSuccess, OnApiFailure);
}

void USequencePayBP::GetSwapPrices(const FString& WalletAddress, const FString& BuyCurrency, const FString& BuyAmount, FOnGetSwapPrices OnSuccess, FOnFailure OnFailure)
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

	this->Pay->GetSwapPrices(SequenceSdk::GetChainId(), WalletAddress, BuyCurrency, BuyAmount, OnApiSuccess, OnApiFailure);
}

void USequencePayBP::GetSwapQuote(const FString& WalletAddress, const FString& BuyCurrency, const FString& SellCurrency, const FString& BuyAmount, const FString& SellAmount, const bool IncludeApprove, FOnGetSwapQuote OnSuccess, FOnFailure OnFailure)
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

	this->Pay->GetSwapQuote(SequenceSdk::GetChainId(), WalletAddress, BuyCurrency, SellCurrency, BuyAmount, SellAmount, IncludeApprove, OnApiSuccess, OnApiFailure);
}

USequencePay* USequencePayBP::GetSequencePay() const
{
	return this->Pay;
}
