#pragma once
#include "Checkout/Structs/TransactionStep.h"
#include "Marketplace/Structs/SeqCollectibleOrder.h"
#include "Marketplace/Structs/SeqCollectiblesFilter.h"
#include "Sequence/SequenceWallet.h"

class FMarketplaceCheckoutTests
{
	static void SubmitStepsAsTransaction(const TArray<FTransactionStep>& Steps, USequenceWallet Wallet);
	static void SeedWalletAndCreateListing(USequenceWallet Wallet, FString Address, FString CurrencyToken);
	static void FetchListingAndBuy(FString Address, USequenceWallet Wallet, int32 Retries, const TSuccessCallback<TArray<FTransactionStep>>& OnSuccess, const FFailureCallback& OnFailure);
	static void FetchListings(FString Collection, int64 ChainId, FSeqCollectiblesFilter Filter, const TSuccessCallback<TArray<FSeqCollectibleOrder>>& OnSuccess, const FFailureCallback& OnFailure);
	static void FetchOfferAndSell(FString CollectionAddress, USequenceWallet Wallet, int32 Retries, const TSuccessCallback<TArray<FTransactionStep>>& OnSuccess, const FFailureCallback& OnFailure);
	static void FetchOffers(FString Collection, int64 ChainId, const TSuccessCallback<TArray<FSeqCollectibleOrder>>& OnSuccess, const FFailureCallback& OnFailure);
	static FSeqOrder FindOrderCreatedByWallet(const FString& WalletAddress, TArray<FSeqCollectibleOrder> CollectibleOrders);
	static void FetchListingsForCollectible(FString Collection, FString TokenId, int64 ChainId, FSeqCollectiblesFilter Filter, const TSuccessCallback<TArray<FSeqOrder>>& OnSuccess, const FFailureCallback& OnFailure);
	
public:
	const FString TestWalletAddress = "0xD2eFbb2f18bfE3D265b26D2ACe83400A65335a07";
	
	static void TestGetCheckoutOptions_Marketplace(int64 Amount);
	static void TestGetCheckoutOptions_PrimarySale_Erc1155(TArray<FString> TokenIds, TArray<int64> Amounts, FString ExpectedException);
	static void TestGetCheckoutOptions_PrimarySale_Erc721(FString TokenId, int64 Amount, FString ExpectedException);
	static void TestGenerateBuyTransaction(int64 Amount);
	static void TestGenerateSellTransaction(int64 Amount);
	static void TestGenerateListingTransaction(int64 Amount);
	static void TestGenerateOfferTransaction(int64 Amount);
	static void TestGenerateOfferTransaction_NativeCurrency();
	static void TestCreateListingAndBuyIt();
	static void TestCreateOfferAndSellIt();
	static void TestCancelOrder();
	static void TestCreateListingAndBuyWithNativeCurrency();
};
