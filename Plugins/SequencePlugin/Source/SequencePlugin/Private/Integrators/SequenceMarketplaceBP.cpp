#include "Integrators/SequenceMarketplaceBP.h"
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

void USequenceMarketplaceBP::GetCollectiblesWithLowestListingsFirstAsync(const int64 ChainId, const FSeqListCollectiblesArgs& Args)
{
	const TSuccessCallback<FSeqListCollectiblesReturn> OnSuccess = [this, ChainId](const FSeqListCollectiblesReturn& CollectiblesWithLowestListings)
		{
			this->CallCollectiblesWithLowestListingsFirstReceived(true, ChainId, CollectiblesWithLowestListings);
		};

	const FFailureCallback OnFailure = [this, ChainId, Args](const FSequenceError& Error)
		{
			SEQ_LOG(Error, TEXT("Error getting Collectibles with Lowest Listings: %s"), *Error.Message);
			FSeqListCollectiblesReturn CollectiblesWithLowestListings;
			this->CallCollectiblesWithLowestListingsFirstReceived(false, ChainId, CollectiblesWithLowestListings);
		};


	this->Marketplace->ListCollectibleListingsWithLowestPriceListingsFirst(ChainId, Args.ContractAddress, Args.Filter, Args.Page, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::GetAllCollectiblesWithLowestListingsFirstAsync(const int64 ChainId,
	const FSeqListCollectiblesArgs& Args)
{
	const TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess = [this, ChainId](const TArray<FSeqCollectibleOrder>& CollectiblesWithLowestListings)
	{
		this->CallAllCollectiblesWithLowestListingsFirstReceived(true, ChainId, CollectiblesWithLowestListings);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting All Collectibles with Lowest Listings First: %s"), *Error.Message);
		const TArray<FSeqCollectibleOrder> CollectiblesWithLowestListings;
		this->CallAllCollectiblesWithLowestListingsFirstReceived(false, ChainId, CollectiblesWithLowestListings);
	};


	this->Marketplace->ListAllCollectibleListingsWithLowestPriceListingsFirst(ChainId, Args.ContractAddress, Args.Filter, OnSuccess, OnFailure);

}

void USequenceMarketplaceBP::GetCollectiblesWithHighestPricedOffersFirstAsync(const int64 ChainId,
	const FSeqListCollectiblesArgs& Args)
{
	const TSuccessCallback<FSeqListCollectiblesReturn> OnSuccess = [this, ChainId](const FSeqListCollectiblesReturn& CollectiblesWithLowestListings)
	{
		this->CallCollectiblesWithHighestPricedOffersFirstReceived(true, ChainId, CollectiblesWithLowestListings);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Collectibles with Highest Priced Offers First: %s"), *Error.Message);
		FSeqListCollectiblesReturn CollectiblesWithHighestOffers;
		this->CallCollectiblesWithHighestPricedOffersFirstReceived(false, ChainId, CollectiblesWithHighestOffers);
	};


	this->Marketplace->ListCollectibleOffersWithHighestPricedOfferFirst(ChainId, Args.ContractAddress, Args.Filter, Args.Page, OnSuccess, OnFailure);

}

void USequenceMarketplaceBP::GetAllCollectiblesWithHighestPricedOffersFirstAsync(const int64 ChainId,
	const FSeqListCollectiblesArgs& Args)
{
	const TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess = [this, ChainId](const TArray<FSeqCollectibleOrder>& CollectiblesWithHighestPricedOffers)
	{
		this->CallAllCollectiblesWithHighestPricedOffersFirstReceived(true, ChainId, CollectiblesWithHighestPricedOffers);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting All Collectibles with Highest Priced Offers First: %s"), *Error.Message);
		const TArray<FSeqCollectibleOrder> CollectiblesWithHighestPricedOffers;
		this->CallAllCollectiblesWithHighestPricedOffersFirstReceived(false, ChainId, CollectiblesWithHighestPricedOffers);
	};


	this->Marketplace->ListAllCollectibleOffersWithHighestPricedOfferFirst(ChainId, Args.ContractAddress, Args.Filter, OnSuccess, OnFailure);

}

void USequenceMarketplaceBP::GetLowestPriceOfferForCollectibleAsync(const int64 ChainId,
	const FSeqGetCollectibleOrderArgs& Args)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnSuccess = [this, ChainId](const FSeqCollectibleOrder& LowestPriceOffer)
	{
		this->CallLowestPriceOfferForCollectibleReceived(true, ChainId, LowestPriceOffer);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Lowest Price Offer for Collectible: %s"), *Error.Message);
		FSeqCollectibleOrder LowestPriceOffer;
		this->CallLowestPriceOfferForCollectibleReceived(false, ChainId, LowestPriceOffer);
	};

	this->Marketplace->GetLowestPriceOfferForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::GetHighestPriceOfferForCollectibleAsync(const int64 ChainId,
	const FSeqGetCollectibleOrderArgs& Args)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnSuccess = [this, ChainId](const FSeqCollectibleOrder& HighestPriceOffer)
	{
		this->CallHighestPriceOfferForCollectibleReceived(true, ChainId, HighestPriceOffer);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Highest Price Offer for Collectible: %s"), *Error.Message);
		FSeqCollectibleOrder HighestPriceOffer;
		this->CallHighestPriceOfferForCollectibleReceived(false, ChainId, HighestPriceOffer);
	};

	this->Marketplace->GetHighestPriceOfferForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::GetLowestPriceListingForCollectibleAsync(const int64 ChainId,
	const FSeqGetCollectibleOrderArgs& Args)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnSuccess = [this, ChainId](const FSeqCollectibleOrder& LowestPriceListing)
	{
		this->CallLowestPriceListingForCollectibleReceived(true, ChainId, LowestPriceListing);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Lowest Price Listing for Collectible: %s"), *Error.Message);
		FSeqCollectibleOrder LowestPriceListing;
		this->CallLowestPriceListingForCollectibleReceived(false, ChainId, LowestPriceListing);
	};

	this->Marketplace->GetLowestPriceListingForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::GetHighestPriceListingForCollectibleAsync(const int64 ChainId,
	const FSeqGetCollectibleOrderArgs& Args)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnSuccess = [this, ChainId](const FSeqCollectibleOrder& HighestPriceListing)
	{
		this->CallHighestPriceListingForCollectibleReceived(true, ChainId, HighestPriceListing);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Highest Price Listing for Collectible: %s"), *Error.Message);
		FSeqCollectibleOrder HighestPriceListing;
		this->CallHighestPriceListingForCollectibleReceived(false, ChainId, HighestPriceListing);
	};

	this->Marketplace->GetHighestPriceListingForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::ListListingsForCollectibleAsync(const int64 ChainId,
	const FSeqListCollectibleListingsArgs& Args)
{
	const TSuccessCallback<FSeqListCollectibleListingsReturn> OnSuccess = [this, ChainId](const FSeqListCollectibleListingsReturn& Listings)
	{
		this->FOnListListingsForCollectibleResponse.Broadcast(true, ChainId, Listings);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Listings for Collectible: %s"), *Error.Message);
		FSeqListCollectibleListingsReturn Listings;
		this->FOnListListingsForCollectibleResponse.Broadcast(false, ChainId, Listings);
	};

	this->Marketplace->ListListingsForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, Args.Page, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::ListAllListingsForCollectibleAsync(const int64 ChainId,
	const FSeqListCollectibleListingsArgs& Args)
{
	const TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess = [this, ChainId](const TArray<FSeqCollectibleOrder>& Listings)
	{
		this->CallListAllListingsForCollectibleReceived(true, ChainId, Listings);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting All Listings for Collectible: %s"), *Error.Message);
		TArray<FSeqCollectibleOrder> Listings;
		this->CallListAllListingsForCollectibleReceived(false, ChainId, Listings);
	};

	this->Marketplace->ListAllListingsForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::ListOffersForCollectibleAsync(const int64 ChainId,
	const FSeqListCollectibleListingsArgs& Args)
{
	const TSuccessCallback<FSeqListCollectibleOffersReturn> OnSuccess = [this, ChainId](const FSeqListCollectibleOffersReturn& Offers)
	{
		this->FOnListOffersForCollectibleResponse.Broadcast(true, ChainId, Offers);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Offers for Collectible: %s"), *Error.Message);
		FSeqListCollectibleOffersReturn Offers;
		this->FOnListOffersForCollectibleResponse.Broadcast(false, ChainId, Offers);
	};

	this->Marketplace->ListOffersForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, Args.Page, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::ListAllOffersForCollectibleAsync(const int64 ChainId,
	const FSeqListCollectibleListingsArgs& Args)
{
	const TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess = [this, ChainId](const TArray<FSeqCollectibleOrder>& Offers)
	{
		this->FOnListAllOffersForCollectibleResponse.Broadcast(true, ChainId, Offers);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting All Offers for Collectible: %s"), *Error.Message);
		TArray<FSeqCollectibleOrder> Offers;
		this->FOnListAllOffersForCollectibleResponse.Broadcast(false, ChainId, Offers);
	};

	this->Marketplace->ListAllOffersForCollectible(ChainId, Args.ContractAddress, Args.TokenID, Args.Filter, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::GetFloorOrderAsync(const int64 ChainId, const FSeqListCollectiblesArgs& Args)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnSuccess = [this, ChainId](const FSeqCollectibleOrder& FloorOrder)
	{
		this->FOnGetFloorOrderResponse.Broadcast(true, ChainId, FloorOrder);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Floor Order: %s"), *Error.Message);
		FSeqCollectibleOrder FloorOrder;
		this->FOnGetFloorOrderResponse.Broadcast(false, ChainId, FloorOrder);
	};

	this->Marketplace->GetFloorOrder(ChainId, Args.ContractAddress, Args.Filter, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::GetSwapPriceAsync(const int64 ChainId, const FGetSwapPriceArgs& Args)
{
	const TSuccessCallback<FSeqSwapPrice> OnSuccess = [this, ChainId](const FSeqSwapPrice& SwapPrice)
	{
		this->FOnGetSwapPriceResponse.Broadcast(true, ChainId, SwapPrice);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Swap Price: %s"), *Error.Message);
		FSeqSwapPrice SwapPrice;
		this->FOnGetSwapPriceResponse.Broadcast(false, ChainId, SwapPrice);
	};

	this->Marketplace->GetSwapPrice(ChainId, Args.BuyAmount, Args.SellCurrencyAddress, Args.BuyAmount, OnSuccess, OnFailure, Args.SlippagePercentage);
}

void USequenceMarketplaceBP::GetSwapPricesAsync(const int64 ChainId, const FGetSwapPricesArgs& Args)
{
	const TSuccessCallback<TArray<FSeqSwapPrice>> OnSuccess = [this, ChainId](const TArray<FSeqSwapPrice>& SwapPrices)
	{
		this->FOnGetSwapPricesResponse.Broadcast(true, ChainId, SwapPrices);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Swap Prices: %s"), *Error.Message);
		TArray<FSeqSwapPrice> SwapPrices;
		this->FOnGetSwapPricesResponse.Broadcast(false, ChainId, SwapPrices);
	};

	this->Marketplace->GetSwapPrices(ChainId, Args.UserAddress, Args.BuyCurrencyAddress, Args.BuyAmount, OnSuccess, OnFailure, Args.SlippagePercentage);
}

void USequenceMarketplaceBP::GetSwapQuoteAsync(const int64 ChainId, const FGetSwapQuoteArgs& Args)
{
	const TSuccessCallback<FSeqSwapQuote> OnSuccess = [this, ChainId](const FSeqSwapQuote& SwapQuote)
	{
		this->FonGetSwapQuoteResponse.Broadcast(true, ChainId, SwapQuote);
	};

	const FFailureCallback OnFailure = [this, ChainId](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Swap Quote: %s"), *Error.Message);
		FSeqSwapQuote SwapQuote;
		this->FonGetSwapQuoteResponse.Broadcast(false, ChainId, SwapQuote);
	};

	this->Marketplace->GetSwapQuote(ChainId, Args.UserAddress, Args.BuyCurrencyAddress, Args.SellCurrencyAddress, Args.BuyAmount, Args.IncludeApprove, OnSuccess, OnFailure, Args.SlippagePercentage);
}

void USequenceMarketplaceBP::CallCollectiblesWithLowestListingsFirstReceived(const bool Status, const int64 ChainId, const FSeqListCollectiblesReturn& Response)
{
	if (this->CollectiblesWithLowestListingsResponse.IsBound())
		this->CollectiblesWithLowestListingsResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: CollectiblesWithLowestListingsFirstResponse"));
}

void USequenceMarketplaceBP::CallAllCollectiblesWithLowestListingsFirstReceived(const bool Status, const int64 ChainId,
	const TArray<FSeqCollectibleOrder>& Response)
{
	if (this->AllCollectiblesWithLowestListingsResponse.IsBound())
		this->AllCollectiblesWithLowestListingsResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: AllCollectiblesWithLowestListingsFirstResponse"));
}

void USequenceMarketplaceBP::CallCollectiblesWithHighestPricedOffersFirstReceived(const bool Status, const int64 ChainId,
	const FSeqListCollectiblesReturn& Response)
{
	if (this->CollectiblesWithHighestListingsResponse.IsBound())
		this->CollectiblesWithHighestListingsResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: CollectiblesWithHighestPricedOffersFirstResponse"));
}

void USequenceMarketplaceBP::CallAllCollectiblesWithHighestPricedOffersFirstReceived(const bool Status,
	const int64 ChainId, const TArray<FSeqCollectibleOrder>& Response)
{
	if (this->AllCollectiblesWithHighestPricedOffersResponse.IsBound())
		this->AllCollectiblesWithHighestPricedOffersResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: AllCollectiblesWithHighestPricedOffersFirstResponse"));
}

void USequenceMarketplaceBP::CallLowestPriceOfferForCollectibleReceived(const bool Status, const int64 ChainId,
	const FSeqCollectibleOrder& Response)
{
	if (this->GetLowestPriceOfferForCollectibleResponse.IsBound())
		this->GetLowestPriceOfferForCollectibleResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: GetLowestPriceOfferForCollectibleResponse"));
}

void USequenceMarketplaceBP::CallHighestPriceOfferForCollectibleReceived(const bool Status, const int64 ChainId,
	const FSeqCollectibleOrder& Response)
{
	if (this->GetHighestPriceOfferForCollectibleResponse.IsBound())
		this->GetHighestPriceOfferForCollectibleResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: GetHighestPriceOfferForCollectibleResponse"));
}

void USequenceMarketplaceBP::CallLowestPriceListingForCollectibleReceived(const bool Status, const int64 ChainId,
	const FSeqCollectibleOrder& Response)
{
	if (this->GetLowestPriceListingForCollectibleResponse.IsBound())
		this->GetLowestPriceListingForCollectibleResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: GetLowestPriceListingForCollectibleResponse"));
}

void USequenceMarketplaceBP::CallHighestPriceListingForCollectibleReceived(const bool Status, const int64 ChainId,
	const FSeqCollectibleOrder& Response)
{
	if (this->GetHighestPriceListingForCollectibleResponse.IsBound())
		this->GetHighestPriceListingForCollectibleResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: GetHighestPriceListingForCollectibleResponse"));
}

void USequenceMarketplaceBP::CallListListingsForCollectibleReceived(const bool Status, const int64 ChainId,
	const FSeqListCollectibleListingsReturn& Response)
{
	if (this->FOnListListingsForCollectibleResponse.IsBound())
		this->FOnListListingsForCollectibleResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: ListListingsForCollectibleResponse"));
}

void USequenceMarketplaceBP::CallListAllListingsForCollectibleReceived(const bool Status, const int64 ChainId,
	const TArray<FSeqCollectibleOrder>& Response)
{
	if (this->FOnListAllListingsForCollectibleResponse.IsBound())
		this->FOnListAllListingsForCollectibleResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: ListAllListingsForCollectibleResponse"));
}

void USequenceMarketplaceBP::CallListOffersForCollectibleReceived(const bool Status, const int64 ChainId,
	const FSeqListCollectibleOffersReturn& Response)
{
	if (this->FOnListOffersForCollectibleResponse.IsBound())
		this->FOnListOffersForCollectibleResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: ListOffersForCollectibleResponse"));
}

void USequenceMarketplaceBP::CallListAllOffersForCollectibleReceived(const bool Status, const int64 ChainId,
	const TArray<FSeqCollectibleOrder>& Response)
{
	if (this->FOnListAllOffersForCollectibleResponse.IsBound())
		this->FOnListAllOffersForCollectibleResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: ListAllOffersForCollectibleResponse"));
}

void USequenceMarketplaceBP::CallGetFloorOrderReceived(const bool Status, const int64 ChainId,
	const FSeqCollectibleOrder& Response)
{
	if (this->FOnGetFloorOrderResponse.IsBound())
		this->FOnGetFloorOrderResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: GetFloorOrderResponse"));
}

void USequenceMarketplaceBP::CallGetSwapPriceReceived(const bool Status, const int64 ChainId,
	const FSeqSwapPrice& Response)
{
	if (this->FOnGetSwapPriceResponse.IsBound())
		this->FOnGetSwapPriceResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: GetSwapPriceResponse"));
}

void USequenceMarketplaceBP::CallGetSwapPricesReceived(const bool Status, const int64 ChainId,
	const TArray<FSeqSwapPrice>& Response)
{
	if (this->FOnGetSwapPricesResponse.IsBound())
		this->FOnGetSwapPricesResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: GetSwapPricesResponse"));
}

void USequenceMarketplaceBP::CallGetSwapQuoteReceived(const bool Status, const int64 ChainId,
	const FSeqSwapQuote& Response)
{
	if (this->FonGetSwapQuoteResponse.IsBound())
		this->FonGetSwapQuoteResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: GetSwapQuoteResponse"));
}
