#include "Subsystems/SequenceMarketplaceBP.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Marketplace/Structs/SeqGetSwapPriceArgs.h"
#include "Marketplace/Structs/SeqGetSwapPricesArgs.h"
#include "Marketplace/Structs/SeqListCollectibleListingsArgs.h"
#include "Util/Log.h"

USequenceMarketplaceBP::USequenceMarketplaceBP()
{
	this->Marketplace = NewObject<USequenceMarketplace>();
}

void USequenceMarketplaceBP::GetCollectiblesWithLowestListingsFirst(const int64 ChainId, const FSeqListCollectiblesArgs& Args, FOnGetCollectiblesWithLowestListingsFirst OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqListCollectiblesReturn> OnApiSuccess = [this, OnSuccess](const FSeqListCollectiblesReturn& CollectiblesWithLowestListings)
	{
		OnSuccess.ExecuteIfBound(CollectiblesWithLowestListings);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Collectibles with Lowest Listings: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Marketplace->ListCollectibleListingsWithLowestPriceListingsFirst(ChainId, Args.ContractAddress, Args.Filter, Args.Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetAllCollectiblesWithLowestListingsFirst(const int64 ChainId,
	const FSeqListCollectiblesArgs& Args, FOnGetAllCollectiblesWithLowestListingsFirst OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<TArray<FSeqCollectibleOrder>> OnApiSuccess = [this, OnSuccess](const TArray<FSeqCollectibleOrder>& CollectiblesWithLowestListings)
	{
		OnSuccess.ExecuteIfBound(CollectiblesWithLowestListings);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting All Collectibles with Lowest Listings First: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Marketplace->ListAllCollectibleListingsWithLowestPriceListingsFirst(ChainId, Args.ContractAddress, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetCollectiblesWithHighestPricedOffersFirst(const int64 ChainId,
	const FSeqListCollectiblesArgs& Args, FOnGetCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqListCollectiblesReturn> OnApiSuccess = [this, OnSuccess](const FSeqListCollectiblesReturn& CollectiblesWithLowestListings)
	{
		OnSuccess.ExecuteIfBound(CollectiblesWithLowestListings);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Collectibles with Highest Priced Offers First: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Marketplace->ListCollectibleOffersWithHighestPricedOfferFirst(ChainId, Args.ContractAddress, Args.Filter, Args.Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetAllCollectiblesWithHighestPricedOffersFirst(const int64 ChainId,
	const FSeqListCollectiblesArgs& Args, FOnGetAllCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<TArray<FSeqCollectibleOrder>> OnApiSuccess = [this, OnSuccess](const TArray<FSeqCollectibleOrder>& CollectiblesWithHighestPricedOffers)
	{
		OnSuccess.ExecuteIfBound(CollectiblesWithHighestPricedOffers);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting All Collectibles with Highest Priced Offers First: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Marketplace->ListAllCollectibleOffersWithHighestPricedOfferFirst(ChainId, Args.ContractAddress, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetLowestPriceOfferForCollectible(const int64 ChainId,
	const FSeqGetCollectibleOrderArgs& Args, FOnGetLowestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnApiSuccess = [this, OnSuccess](const FSeqCollectibleOrder& LowestPriceOffer)
	{
		OnSuccess.ExecuteIfBound(LowestPriceOffer);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Lowest Price Offer for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetLowestPriceOfferForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetHighestPriceOfferForCollectible(const int64 ChainId,
	const FSeqGetCollectibleOrderArgs& Args, FOnGetHighestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnApiSuccess = [this, OnSuccess](const FSeqCollectibleOrder& HighestPriceOffer)
	{
		OnSuccess.ExecuteIfBound(HighestPriceOffer);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Highest Price Offer for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetHighestPriceOfferForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetLowestPriceListingForCollectible(const int64 ChainId,
	const FSeqGetCollectibleOrderArgs& Args, FOnGetLowestPriceListingForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnApiSuccess = [this, OnSuccess](const FSeqCollectibleOrder& LowestPriceListing)
	{
		OnSuccess.ExecuteIfBound(LowestPriceListing);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Lowest Price Listing for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetLowestPriceListingForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetHighestPriceListingForCollectible(const int64 ChainId,
	const FSeqGetCollectibleOrderArgs& Args, FOnGetHighestPriceListingForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnApiSuccess = [this, OnSuccess](const FSeqCollectibleOrder& HighestPriceListing)
	{
		OnSuccess.ExecuteIfBound(HighestPriceListing);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Highest Price Listing for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetHighestPriceListingForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListListingsForCollectible(const int64 ChainId,
	const FSeqListCollectibleListingsArgs& Args, FOnListListingsForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqListCollectibleListingsReturn> OnApiSuccess = [this, OnSuccess](const FSeqListCollectibleListingsReturn& Listings)
	{
		OnSuccess.ExecuteIfBound(Listings);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Listings for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->ListListingsForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, Args.Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListAllListingsForCollectible(const int64 ChainId,
	const FSeqListCollectibleListingsArgs& Args, FOnListAllListingsForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<TArray<FSeqCollectibleOrder>> OnApiSuccess = [this, OnSuccess](const TArray<FSeqCollectibleOrder>& Listings)
	{
		OnSuccess.ExecuteIfBound(Listings);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting All Listings for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->ListAllListingsForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListOffersForCollectible(const int64 ChainId,
	const FSeqListCollectibleListingsArgs& Args, FOnListOffersForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqListCollectibleOffersReturn> OnApiSuccess = [this, OnSuccess](const FSeqListCollectibleOffersReturn& Offers)
	{
		OnSuccess.ExecuteIfBound(Offers);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Offers for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->ListOffersForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, Args.Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListAllOffersForCollectible(const int64 ChainId,
	const FSeqListCollectibleListingsArgs& Args, FOnListAllOffersForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<TArray<FSeqCollectibleOrder>> OnApiSuccess = [this, OnSuccess](const TArray<FSeqCollectibleOrder>& Offers)
	{
		OnSuccess.ExecuteIfBound(Offers);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting All Offers for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->ListAllOffersForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetFloorOrder(const int64 ChainId, const FSeqListCollectiblesArgs& Args, FOnGetFloorOrder OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnApiSuccess = [this, OnSuccess](const FSeqCollectibleOrder& FloorOrder)
	{
		OnSuccess.ExecuteIfBound(FloorOrder);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Floor Order: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetFloorOrder(ChainId, Args.ContractAddress, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetSwapPrice(const int64 ChainId, const FGetSwapPriceArgs& Args, FOnGetSwapPrice OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqSwapPrice> OnApiSuccess = [this, OnSuccess](const FSeqSwapPrice& SwapPrice)
	{
		OnSuccess.ExecuteIfBound(SwapPrice);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Swap Price: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetSwapPrice(ChainId, Args.BuyAmount, Args.SellCurrencyAddress, Args.BuyAmount, OnApiSuccess, OnApiFailure, Args.SlippagePercentage);
}

void USequenceMarketplaceBP::GetSwapPrices(const int64 ChainId, const FGetSwapPricesArgs& Args, FOnGetSwapPrices OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->GetSwapPrices(ChainId, Args.UserAddress, Args.BuyCurrencyAddress, Args.BuyAmount, OnApiSuccess, OnApiFailure, Args.SlippagePercentage);
}

void USequenceMarketplaceBP::GetSwapQuote(const int64 ChainId, const FGetSwapQuoteArgs& Args, FOnGetSwapQuote OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqSwapQuote> OnApiSuccess = [this, OnSuccess](const FSeqSwapQuote& SwapQuote)
	{
		OnSuccess.ExecuteIfBound(SwapQuote);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Swap Quote: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetSwapQuote(ChainId, Args.UserAddress, Args.BuyCurrencyAddress, Args.SellCurrencyAddress, Args.BuyAmount, Args.IncludeApprove, OnApiSuccess, OnApiFailure, Args.SlippagePercentage);
}
