#pragma once
#include "Checkout/Structs/AdditionalFee.h"
#include "Checkout/Structs/CheckoutOptionsMarketplaceOrder.h"
#include "Checkout/Structs/GenerateTransactionResponse.h"
#include "Checkout/Structs/GetCheckoutOptionsResponse.h"
#include "Marketplace/Marketplace.h"
#include "Sequence/SequenceAPI.h"

#include "Checkout.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UCheckout : public UObject
{
	GENERATED_BODY()
	
private:
	UMarketplace* Marketplace = nullptr;
	USequenceWallet* Wallet = nullptr;
	int64 ChainID;
	
public:
	// Constructor
	UCheckout();
	UCheckout(UMarketplace* Marketplace, USequenceWallet* InWallet, const int64& InChainID);

	template < typename T> FString BuildArgs(T StructIn) const;

	template < typename T > T BuildResponse(FString Text) const;

	void GetCheckoutOptions(const TArray<FCheckoutOptionsMarketplaceOrder>& Orders, int64 AdditionalFeeBps, TSuccessCallback<FGetCheckoutOptionsResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	void GetCheckoutOptions(const TArray<FSeqOrder>& Orders, int64 AdditionalFeeBps, const TSuccessCallback<FGetCheckoutOptionsResponse>& OnSuccess, const FFailureCallback& OnFailure) const;
	void GenerateBuyTransaction(const FSeqOrder& Order, int64 Amount, const FAdditionalFee& AdditionalFee, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	void GenerateSellTransaction(const FSeqOrder& Order, int64 Amount, const FAdditionalFee& AdditionalFee, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	void GenerateListingTransaction(const FString& CollectionAddress, const FString& TokenId, int64 Amount, EContractType ContractType, const FString& CurrencyTokenAddress, int64 PricePerToken, FDateTime Expiry, EOrderbookKind OrderbookKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	void GenerateOfferTransaction(const FString& CollectionAddress, const FString& TokenId, int64 Amount, EContractType ContractType, const FString& CurrencyTokenAddress, int64 PricePerToken, FDateTime Expiry, EOrderbookKind OrderbookKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	void GenerateCancelTransaction(const FString& CollectionAddress, const FString& OrderId, EMarketplaceKind MarketplaceKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	void GenerateCancelTransaction(const FString& CollectionAddress, const FSeqOrder& Order, EMarketplaceKind MarketplaceKind, const TSuccessCallback<FGenerateTransactionResponse>& OnSuccess, const FFailureCallback& OnFailure) const;
};
