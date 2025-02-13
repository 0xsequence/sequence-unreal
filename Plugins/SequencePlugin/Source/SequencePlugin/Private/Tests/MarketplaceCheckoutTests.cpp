#include "MarketplaceCheckoutTests.h"

// Private

void FMarketplaceCheckoutTests::SubmitStepsAsTransaction(const TArray<FTransactionStep>& Steps, USequenceWallet Wallet)
{
	if (Steps.IsEmpty())
	{
		UE_LOG(LogTemp,Display,TEXT("Transaction Steps Is EMPTY"));
		return;
	}

	
}

void FMarketplaceCheckoutTests::SeedWalletAndCreateListing(USequenceWallet Wallet, FString Address,
	FString CurrencyToken)
{
}

void FMarketplaceCheckoutTests::FetchListingAndBuy(FString Address, USequenceWallet Wallet, int32 Retries,
	const TSuccessCallback<TArray<FTransactionStep>>& OnSuccess, const FFailureCallback& OnFailure)
{
}

void FMarketplaceCheckoutTests::FetchListings(FString Collection, int64 ChainId, FSeqCollectiblesFilter Filter,
	const TSuccessCallback<TArray<FSeqCollectibleOrder>>& OnSuccess, const FFailureCallback& OnFailure)
{
}

void FMarketplaceCheckoutTests::FetchOfferAndSell(FString CollectionAddress, USequenceWallet Wallet, int32 Retries,
	const TSuccessCallback<TArray<FTransactionStep>>& OnSuccess, const FFailureCallback& OnFailure)
{
}

void FMarketplaceCheckoutTests::FetchOffers(FString Collection, int64 ChainId,
	const TSuccessCallback<TArray<FSeqCollectibleOrder>>& OnSuccess, const FFailureCallback& OnFailure)
{
}

FSeqOrder FMarketplaceCheckoutTests::FindOrderCreatedByWallet(const FString& WalletAddress,
	TArray<FSeqCollectibleOrder> CollectibleOrders)
{
	for (auto [TokenMetadata, Order] : CollectibleOrders)
	{
		if (Order.CreatedBy == WalletAddress)
		{
			return Order;
		}
	}

	return FSeqOrder();
}

void FMarketplaceCheckoutTests::FetchListingsForCollectible(FString Collection, FString TokenId, int64 ChainId,
	FSeqCollectiblesFilter Filter, const TSuccessCallback<TArray<FSeqOrder>>& OnSuccess,
	const FFailureCallback& OnFailure)
{
}

// Public

void FMarketplaceCheckoutTests::TestGetCheckoutOptions_Marketplace(int64 Amount)
{
}

void FMarketplaceCheckoutTests::TestGetCheckoutOptions_PrimarySale_ERC1155(TArray<FString> TokenIds,
	TArray<int64> Amounts, FString ExpectedException)
{
}

void FMarketplaceCheckoutTests::TestGetCheckoutOptions_PrimarySale_ERC721(FString TokenId, int64 Amount,
	FString ExpectedException)
{
}

void FMarketplaceCheckoutTests::TestGenerateBuyTransaction(int64 Amount)
{
}

void FMarketplaceCheckoutTests::TestGenerateSellTransaction(int64 Amount)
{
}

void FMarketplaceCheckoutTests::TestGenerateListingTransaction(int64 Amount)
{
}

void FMarketplaceCheckoutTests::TestGenerateOfferTransaction(int64 Amount)
{
}

void FMarketplaceCheckoutTests::TestGenerateOfferTransaction_NativeCurrency()
{
}

void FMarketplaceCheckoutTests::TestCreateListingAndBuyIt()
{
}

void FMarketplaceCheckoutTests::TestCreateOfferAndSellIt()
{
}

void FMarketplaceCheckoutTests::TestCancelOrder()
{
}

void FMarketplaceCheckoutTests::TestCreateListingAndBuyWithNativeCurrency()
{
}
