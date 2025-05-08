// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TransakDataTypes.h"
#include "Marketplace/Structs/SeqSwapPrice.h"
#include "Marketplace/Structs/SeqSwapQuote.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "SequencePay.generated.h"

constexpr int DefaultSlippagePercentage = 5;

UCLASS()
class SEQUENCEPLUGIN_API USequencePay : public UObject
{
	GENERATED_BODY()

public:
	FString GetOnRampLink(const FString& WalletAddress, const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn);
	
	void OpenOnRampInExternalBrowser(const FString& WalletAddress, const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn);

	void GetSupportedOnRampCountries(TSuccessCallback<TArray<FSupportedCountry>> OnSuccess, FFailureCallback OnFailure);
	
	/**
	 * 
	 * @param ChainID the id of the chain
	 * @param SellCurrency the address of the currency to sell
	 * @param BuyCurrency the address of the currency to buy
	 * @param BuyAmount the amount to buy
	 * @param OnSuccess handler for success, takes in a FSeqSwapPrice
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 * @param SlippagePercentage the slippage percentage
	 */
	void GetSwapPrice(const int64 ChainID, const FString& SellCurrency, const FString& BuyCurrency, const FString& BuyAmount, const TSuccessCallback<FSeqSwapPrice>& OnSuccess, const FFailureCallback& OnFailure, const int SlippagePercentage = DefaultSlippagePercentage);

	/**
	 * 
	 * @param ChainID the id of the chain
	 * @param UserWallet the address of the user wallet
	 * @param BuyCurrency the address of the currency to buy
	 * @param BuyAmount the amount to buy
	 * @param OnSuccess handler for success, takes in a TArray of FSeqSwapPrice
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 * @param SlippagePercentage the slippage percentage
	 */
	void GetSwapPrices(const int64 ChainID, const FString& UserWallet, const FString& BuyCurrency, const FString& BuyAmount, const TSuccessCallback<TArray<FSeqSwapPrice>>& OnSuccess, const FFailureCallback& OnFailure, const int SlippagePercentage = DefaultSlippagePercentage);

	/**
	 * 
	 * @param ChainID the id of the chain
	 * @param UserWallet the address of the user wallet
	 * @param BuyCurrency the address of the currency to buy
	 * @param SellCurrency the address of the currency to sell
	 * @param BuyAmount the amount to buy
	 * @param IncludeApprove whether to include approve
	 * @param OnSuccess handler for success, takes in a FSeqSwapQuote
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 * @param SlippagePercentage the slippage percentage
	 */
	void GetSwapQuote(const int64 ChainID, const FString& UserWallet, const FString& BuyCurrency, const FString& SellCurrency, const FString& BuyAmount, const bool IncludeApprove, const TSuccessCallback<FSeqSwapQuote>& OnSuccess, const FFailureCallback& OnFailure, const int SlippagePercentage = DefaultSlippagePercentage);
	
private:
	/*
	Here we take in a struct and convert it straight into a json object String
	@Param (T) Struct_in the struct we are converting to a json object string
	@Return the JSON Object String
	*/
	template < typename T> FString BuildArgs(T StructIn);
	
	void HTTPPostSwapAPI(const FString& Endpoint, const FString& Args, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;
	void AssertWeHaveSufficientBalance(const int64 ChainID, const FString& UserWallet, const FString& BuyCurrency, const FString& SellCurrency, const FString& BuyAmount, const TFunction<void ()>& OnSuccess, const FFailureCallback& OnFailure, const int SlippagePercentage = DefaultSlippagePercentage);
};
