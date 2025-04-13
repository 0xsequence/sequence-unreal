#include "Subsystems/SequenceMarketplaceBP.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Marketplace/Structs/SeqGetSwapPriceArgs.h"
#include "Marketplace/Structs/SeqGetSwapPricesArgs.h"
#include "Marketplace/Structs/SeqListCollectibleListingsArgs.h"
#include "Sequence/SequenceSdk.h"
#include "Util/Log.h"

USequenceMarketplaceBP::USequenceMarketplaceBP()
{
	this->Marketplace = NewObject<USequenceMarketplace>();
}

void USequenceMarketplaceBP::GetCollectiblesWithLowestListingsFirst(const FSeqListCollectiblesArgs& Args, FOnGetCollectiblesWithLowestListingsFirst OnSuccess, FOnFailure OnFailure)
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
	
	this->Marketplace->ListCollectibleListingsWithLowestPriceListingsFirst(SequenceSdk::GetChainId(), Args.ContractAddress, Args.Filter, Args.Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetAllCollectiblesWithLowestListingsFirst(const FSeqListCollectiblesArgs& Args, FOnGetAllCollectiblesWithLowestListingsFirst OnSuccess, FOnFailure OnFailure)
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
	
	this->Marketplace->ListAllCollectibleListingsWithLowestPriceListingsFirst(SequenceSdk::GetChainId(), Args.ContractAddress, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetCollectiblesWithHighestPricedOffersFirst(const FSeqListCollectiblesArgs& Args, FOnGetCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure)
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
	
	this->Marketplace->ListCollectibleOffersWithHighestPricedOfferFirst(SequenceSdk::GetChainId(), Args.ContractAddress, Args.Filter, Args.Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetAllCollectiblesWithHighestPricedOffersFirst(const FSeqListCollectiblesArgs& Args, FOnGetAllCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure)
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
	
	this->Marketplace->ListAllCollectibleOffersWithHighestPricedOfferFirst(SequenceSdk::GetChainId(), Args.ContractAddress, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetLowestPriceOfferForCollectible(const FSeqGetCollectibleOrderArgs& Args, FOnGetLowestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->GetLowestPriceOfferForCollectible(SequenceSdk::GetChainId(), Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetHighestPriceOfferForCollectible(const FSeqGetCollectibleOrderArgs& Args, FOnGetHighestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->GetHighestPriceOfferForCollectible(SequenceSdk::GetChainId(), Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetLowestPriceListingForCollectible(const FSeqGetCollectibleOrderArgs& Args, FOnGetLowestPriceListingForCollectible OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->GetLowestPriceListingForCollectible(SequenceSdk::GetChainId(), Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetHighestPriceListingForCollectible(const FSeqGetCollectibleOrderArgs& Args, FOnGetHighestPriceListingForCollectible OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->GetHighestPriceListingForCollectible(SequenceSdk::GetChainId(), Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListListingsForCollectible(const FSeqListCollectibleListingsArgs& Args, FOnListListingsForCollectible OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->ListListingsForCollectible(SequenceSdk::GetChainId(), Args.ContractAddress, Args.TokenID, Args.Filter, Args.Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListAllListingsForCollectible(const FSeqListCollectibleListingsArgs& Args, FOnListAllListingsForCollectible OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->ListAllListingsForCollectible(SequenceSdk::GetChainId(), Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListOffersForCollectible(const FSeqListCollectibleListingsArgs& Args, FOnListOffersForCollectible OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->ListOffersForCollectible(SequenceSdk::GetChainId(), Args.ContractAddress, Args.TokenID, Args.Filter, Args.Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListAllOffersForCollectible(const FSeqListCollectibleListingsArgs& Args, FOnListAllOffersForCollectible OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->ListAllOffersForCollectible(SequenceSdk::GetChainId(), Args.ContractAddress, Args.TokenID, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetFloorOrder(const FSeqListCollectiblesArgs& Args, FOnGetFloorOrder OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->GetFloorOrder(SequenceSdk::GetChainId(), Args.ContractAddress, Args.Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetSwapPrice(const FGetSwapPriceArgs& Args, FOnGetSwapPrice OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->GetSwapPrice(SequenceSdk::GetChainId(), Args.BuyAmount, Args.SellCurrencyAddress, Args.BuyAmount, OnApiSuccess, OnApiFailure, Args.SlippagePercentage);
}

void USequenceMarketplaceBP::GetSwapPrices(const FGetSwapPricesArgs& Args, FOnGetSwapPrices OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->GetSwapPrices(SequenceSdk::GetChainId(), Args.UserAddress, Args.BuyCurrencyAddress, Args.BuyAmount, OnApiSuccess, OnApiFailure, Args.SlippagePercentage);
}

void USequenceMarketplaceBP::GetSwapQuote(const FGetSwapQuoteArgs& Args, FOnGetSwapQuote OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->GetSwapQuote(SequenceSdk::GetChainId(), Args.UserAddress, Args.BuyCurrencyAddress, Args.SellCurrencyAddress, Args.BuyAmount, Args.IncludeApprove, OnApiSuccess, OnApiFailure, Args.SlippagePercentage);
}
