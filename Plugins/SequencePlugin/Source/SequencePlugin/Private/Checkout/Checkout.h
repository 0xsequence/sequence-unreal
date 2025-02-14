#pragma once
#include "Errors.h"
#include "Checkout/Structs/AdditionalFee.h"
#include "Checkout/Structs/CheckoutOptionsMarketplaceOrder.h"
#include "Checkout/Structs/GenerateTransactionResponse.h"
#include "Checkout/Structs/GetCheckoutOptionsResponse.h"
#include "Marketplace/Marketplace.h"
#include "Sequence/SequenceAPI.h"

#include "Checkout.generated.h"

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetCheckoutOptionsResponseSuccess, FGetCheckoutOptionsResponse, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGenerateTransactionResponseSuccess, FGenerateTransactionResponse, Response);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE(FOnCheckoutFailure);

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
	
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptions(const TArray<FCheckoutOptionsMarketplaceOrder>& Orders, int64 AdditionalFeeBps, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptionsByOrders(const TArray<FSeqOrder>& Orders, const int64 AdditionalFeeBps, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure)const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateBuyTransaction(const FSeqOrder& Order, const int64 Amount, const FAdditionalFee& AdditionalFee, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateSellTransaction(const FSeqOrder& Order, const int64 Amount, const FAdditionalFee& AdditionalFee, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateListingTransaction(const FString& CollectionAddress, const FString& TokenId, int64 Amount, EContractType ContractType, const FString& CurrencyTokenAddress, int64 PricePerToken, FDateTime Expiry, EOrderbookKind OrderbookKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateOfferTransaction(const FString& CollectionAddress, const FString& TokenId, int64 Amount, EContractType ContractType, const FString& CurrencyTokenAddress, int64 PricePerToken, FDateTime Expiry, EOrderbookKind OrderbookKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateCancelTransaction(const FString& CollectionAddress, const FString& OrderId, EMarketplaceKind MarketplaceKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateCancelTransactionByOrder(const FString& CollectionAddress, const FSeqOrder& Order, const EMarketplaceKind MarketplaceKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
};
