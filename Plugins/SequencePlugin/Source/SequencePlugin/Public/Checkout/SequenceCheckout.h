#pragma once
#include "Errors.h"
#include "Checkout/Structs/AdditionalFee.h"
#include "Checkout/Structs/CheckoutOptionsMarketplaceOrder.h"
#include "Checkout/Structs/GenerateTransactionResponse.h"
#include "Checkout/Structs/GetCheckoutOptionsResponse.h"
#include "Marketplace/Marketplace.h"
#include "Sequence/SequenceAPI.h"
#include "Sequence/Wallet_Enums.h"
#include "Types/ERC1155SaleContract.h"
#include "Types/ERC721SaleContract.h"

#include "SequenceCheckout.generated.h"

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGetCheckoutOptionsResponseSuccess, FGetCheckoutOptionsResponse, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnGenerateTransactionResponseSuccess, FGenerateTransactionResponse, Response);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE(FOnCheckoutFailure);

UCLASS()
class SEQUENCEPLUGIN_API USequenceCheckout : public UObject
{
	GENERATED_BODY()
	
private:
	int64 ChainID;

	const FString Path = "/rpc/Marketplace/";
	
	FString Url(const int64& TargetChainID,const FString& EndPoint) const;
	static FString HostName(int64 TargetChainID);
	void HTTPPost(const int64& TargetChainID,const FString& Endpoint,const FString& Args, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;
	
public:
	// Constructor
	USequenceCheckout();
	explicit USequenceCheckout(const int64& InChainID);

	template < typename T> FString BuildArgs(T StructIn) const;

	template < typename T > T BuildResponse(FString Text) const;

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SetChainID(const int64& InChainID);
	
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptions(const FString& WalletAddress, const TArray<FCheckoutOptionsMarketplaceOrder>& Orders, int64 AdditionalFeeBps, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptionsByOrders(const FString& WalletAddress, const TArray<FSeqOrder>& Orders, const int64 AdditionalFeeBps, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptionsByTokenIdAmounts(const FString& WalletAddress, const UERC1155SaleContract* SaleContract, const FString& CollectionAddress, TMap<FString, int64> AmountsByTokenId, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptionsByERC1155Contract(const FString& WalletAddress, const UERC1155SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId, const int64 Amount, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptionsByERC721Contract(const FString& WalletAddress, const UERC721SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId, const int64 Amount, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	
	UFUNCTION(BlueprintCallable, Category = "Checkout", meta = (ExpandEnumAsExecs = "WalletKind"))
	void GenerateBuyTransaction(const FString& WalletAddress, const FSeqOrder& Order, const int64 Amount, const FAdditionalFee& AdditionalFee, EWalletKind WalletKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateSellTransaction(const FString& WalletAddress, const FSeqOrder& Order, const int64 Amount, const FAdditionalFee& AdditionalFee, EWalletKind WalletKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateListingTransaction(const FString& WalletAddress, const FString& CollectionAddress, const FString& TokenId, int64 Amount, EContractType ContractType, const FString& CurrencyTokenAddress, int64 PricePerToken, FDateTime Expiry, EOrderbookKind OrderbookKind, const EWalletKind WalletKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateOfferTransaction(const FString& WalletAddress, const FString& CollectionAddress, const FString& TokenId, int64 Amount, EContractType ContractType, const FString& CurrencyTokenAddress, int64 PricePerToken, FDateTime Expiry, EOrderbookKind OrderbookKind, const EWalletKind WalletKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateCancelTransaction(const FString& WalletAddress, const FString& CollectionAddress, const FString& OrderId, EMarketplaceKind MarketplaceKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateCancelTransactionByOrder(const FString& WalletAddress, const FString& CollectionAddress, const FSeqOrder& Order, EMarketplaceKind MarketplaceKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
};
