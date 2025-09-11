#pragma once
#include "Errors.h"
#include "Checkout/Structs/AdditionalFee.h"
#include "Checkout/Structs/CheckoutOptionsMarketplaceOrder.h"
#include "Checkout/Structs/GenerateTransactionResponse.h"
#include "Checkout/Structs/GetCheckoutOptionsResponse.h"
#include "Marketplace/SequenceMarketplace.h"
#include "Sequence/SequenceEmbeddedWallet.h"
#include "Sequence/Wallet_Enums.h"
#include "Types/ERC1155SaleContract.h"
#include "Types/ERC721SaleContract.h"

#include "SequenceCheckout.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API USequenceCheckout : public UObject
{
	GENERATED_BODY()
	
private:
	const FString Path = "/rpc/Marketplace/";
	
	FString Url(const int64& TargetChainID,const FString& EndPoint) const;
	static FString HostName(int64 TargetChainID);
	void HTTPPost(const int64& TargetChainID,const FString& Endpoint,const FString& Args, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;
	
public:
	template < typename T> FString BuildArgs(T StructIn) const;

	template < typename T > T BuildResponse(FString Text) const;
	
	void GetCheckoutOptions(const int64 ChainID, const FString& WalletAddress, const TArray<FCheckoutOptionsMarketplaceOrder>& Orders, int64 AdditionalFeeBps, TSuccessCallback<FGetCheckoutOptionsResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	
	void GetCheckoutOptionsByOrders(const int64 ChainID, const FString& WalletAddress, const TArray<FSeqOrder>& Orders, const int64 AdditionalFeeBps, TSuccessCallback<FGetCheckoutOptionsResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	
	void GetCheckoutOptionsByTokenIdAmounts(const int64 ChainID, const FString& WalletAddress, const UERC1155SaleContract* SaleContract, const FString& CollectionAddress, TMap<FString, int64> AmountsByTokenId, TSuccessCallback<FGetCheckoutOptionsResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	
	void GetCheckoutOptionsByERC1155Contract(const int64 ChainID, const FString& WalletAddress, const UERC1155SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId, const int64 Amount, TSuccessCallback<FGetCheckoutOptionsResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	
	void GetCheckoutOptionsByERC721Contract(const int64 ChainID, const FString& WalletAddress, const UERC721SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId, const int64 Amount, TSuccessCallback<FGetCheckoutOptionsResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	
	void GenerateBuyTransaction(const int64 ChainID, const FString& WalletAddress, const FSeqOrder& Order, const int64 Amount, const FAdditionalFee& AdditionalFee, EWalletKind WalletKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	
	void GenerateSellTransaction(const int64 ChainID, const FString& WalletAddress, const FSeqOrder& Order, const int64 Amount, const FAdditionalFee& AdditionalFee, EWalletKind WalletKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	
	void GenerateListingTransaction(const int64 ChainID, const FString& WalletAddress, const FString& CollectionAddress, const FString& TokenId, int64 Amount, EContractType ContractType, const FString& CurrencyTokenAddress, int64 PricePerToken, FDateTime Expiry, EOrderbookKind OrderbookKind, const EWalletKind WalletKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const;
	
	void GenerateOfferTransaction(const int64 ChainID, const FString& WalletAddress, const FString& CollectionAddress, const FString& TokenId, int64 Amount, EContractType ContractType, const FString& CurrencyTokenAddress, int64 PricePerToken, FDateTime Expiry, EOrderbookKind OrderbookKind, const EWalletKind WalletKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const;

	void GenerateCancelTransaction(const int64 ChainID, const FString& WalletAddress, const FString& CollectionAddress, const FString& OrderId, EMarketplaceKind MarketplaceKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const;

	void GenerateCancelTransactionByOrder(const int64 ChainID, const FString& WalletAddress, const FString& CollectionAddress, const FSeqOrder& Order, EMarketplaceKind MarketplaceKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure);
};
