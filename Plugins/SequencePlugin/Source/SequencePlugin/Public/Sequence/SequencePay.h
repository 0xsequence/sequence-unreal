#pragma once

#include "CoreMinimal.h"
#include "TransakDataTypes.h"
#include "Marketplace/Structs/SeqGetLifiChainsResponse.h"
#include "Marketplace/Structs/SeqGetLifiTokensResponse.h"
#include "Marketplace/Structs/SeqSwapPrice.h"
#include "Marketplace/Structs/SeqSwapQuote.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "SequencePay.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API USequencePay : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Generates a URL for initiating an on-ramp (fiat-to-crypto) transaction with the specified parameters.
	 *
	 * @param WalletAddress The user's wallet address to be used as the destination for the crypto funds.
	 * @param FiatCurrencyIn The fiat currency code (e.g., "USD", "EUR") to be used for the transaction.
	 * @param DefaultFiatAmountIn The default fiat amount to pre-fill in the on-ramp service.
	 * @param DefaultCryptoCurrencyIn The default cryptocurrency to receive (e.g., "ETH", "USDC").
	 * @param NetworksIn A comma-separated list of supported blockchain networks (e.g., "ethereum,polygon").
	 * @param DisableWalletAddressFormIn If true, the wallet address input will be disabled in the on-ramp UI.
	 * @return A pre-configured URL to launch the on-ramp flow in an external browser or webview.
	 */
	FString GetOnRampLink(const FString& WalletAddress, const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn);

	/**
	 * Opens an external browser window to initiate an on-ramp (fiat-to-crypto) transaction with pre-filled parameters.
	 *
	 * @param WalletAddress The user's wallet address to be used as the crypto destination.
	 * @param FiatCurrencyIn The fiat currency code (e.g., "USD", "EUR") for the transaction.
	 * @param DefaultFiatAmountIn The default amount in fiat currency to pre-fill in the on-ramp interface.
	 * @param DefaultCryptoCurrencyIn The default cryptocurrency to receive (e.g., "ETH", "BTC").
	 * @param NetworksIn A comma-separated list of blockchain networks to include (e.g., "ethereum,polygon").
	 * @param DisableWalletAddressFormIn If true, the wallet address field will be disabled in the on-ramp UI.
	 */
	void OpenOnRampInExternalBrowser(const FString& WalletAddress, const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn);

	/**
	 * Retrieves the list of countries supported for on-ramp services (e.g., fiat-to-crypto conversions).
	 *
	 * @param OnSuccess Callback invoked upon successful retrieval. Receives an array of FSupportedCountry objects.
	 * @param OnFailure Callback invoked if the request fails, providing error information or diagnostics.
	 */
	void GetSupportedOnRampCountries(TSuccessCallback<TArray<FSupportedCountry>> OnSuccess, FFailureCallback OnFailure);

	/**
	 * Retrieves the list of supported blockchain networks (chains) for token swaps.
	 *
	 * @param OnSuccess Callback invoked upon successful retrieval of chain data. Receives a FSeqGetLifiChainsResponse object.
	 * @param OnFailure Callback invoked if the request fails, providing error context or diagnostics.
	 */
	void GetSupportedSwapChains(const TSuccessCallback<FSeqGetLifiChainsResponse>& OnSuccess, const FFailureCallback& OnFailure) const;

	/**
	 * Retrieves the list of supported swap tokens for the specified blockchain networks.
	 *
	 * @param ChainIds An array of blockchain chain IDs for which to fetch the supported tokens.
	 * @param OnSuccess Callback invoked upon successful retrieval of token data. Receives a FSeqGetLifiTokensResponse object.
	 * @param OnFailure Callback invoked if the request fails, typically providing error context or logging.
	 */
	void GetSupportedSwapTokens(const TArray<int64>& ChainIds, const TSuccessCallback<FSeqGetLifiTokensResponse>& OnSuccess, const FFailureCallback& OnFailure);
	
	/**
	 * 
	 * @param ChainID the id of the chain
	 * @param WalletAddress the address of the user wallet
	 * @param SellCurrency the address of the currency to sell
	 * @param BuyCurrency the address of the currency to buy
	 * @param BuyAmount the amount to buy
	 * @param OnSuccess handler for success, takes in a FSeqSwapPrice
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void GetSwapPrice(const int64 ChainID, const FString& WalletAddress, const FString& SellCurrency, const FString& BuyCurrency, const FString& BuyAmount, const TSuccessCallback<FSeqSwapPrice>& OnSuccess, const FFailureCallback& OnFailure);
	
	/**
	 * 
	 * @param ChainID the id of the chain
	 * @param WalletAddress the address of the user wallet
	 * @param BuyCurrency the address of the currency to buy
	 * @param BuyAmount the amount to buy
	 * @param OnSuccess handler for success, takes in a TArray of FSeqSwapPrice
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void GetSwapPrices(const int64 ChainID, const FString& WalletAddress, const FString& BuyCurrency, const FString& BuyAmount, const TSuccessCallback<TArray<FSeqSwapPrice>>& OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * 
	 * @param ChainID the id of the chain
	 * @param WalletAddress the address of the user wallet
	 * @param BuyCurrency the address of the currency to buy
	 * @param SellCurrency the address of the currency to sell
	 * @param BuyAmount the amount to buy
	 * @param SellAmount the amount to sell
	 * @param IncludeApprove whether to include approve
	 * @param OnSuccess handler for success, takes in a FSeqSwapQuote
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void GetSwapQuote(const int64 ChainID, const FString& WalletAddress, const FString& BuyCurrency, const FString& SellCurrency, const FString& BuyAmount, const FString& SellAmount, const bool IncludeApprove, const TSuccessCallback<FSeqSwapQuote>& OnSuccess, const FFailureCallback& OnFailure);
	
private:
	/*
	Here we take in a struct and convert it straight into a json object String
	@Param (T) Struct_in the struct we are converting to a json object string
	@Return the JSON Object String
	*/
	template < typename T> FString BuildArgs(T StructIn);
	
	void HTTPPostSwapAPI(const FString& Endpoint, const FString& Args, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;
};
