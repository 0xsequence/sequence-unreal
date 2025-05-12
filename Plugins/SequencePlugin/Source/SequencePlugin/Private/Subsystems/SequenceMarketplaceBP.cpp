#include "Subsystems/SequenceMarketplaceBP.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Marketplace/Structs/SeqListCollectibleListingsArgs.h"
#include "Sequence/SequenceSdk.h"
#include "Util/Log.h"

USequenceMarketplaceBP::USequenceMarketplaceBP()
{
	this->Marketplace = NewObject<USequenceMarketplace>();
}

void USequenceMarketplaceBP::ListCurrencies(FOnListCurrencies OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqListCurrenciesReturn> OnApiSuccess = [this, OnSuccess](const FSeqListCurrenciesReturn& Response)
	{
		OnSuccess.ExecuteIfBound(Response.Currencies);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Collectibles with Lowest Listings: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Marketplace->ListCurrencies(SequenceSdk::GetChainId(), OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetCollectiblesWithLowestListingsFirst(const FString& ContractAddress, const FSeqCollectiblesFilter Filter, const FSeqMarketplacePage Page, FOnGetCollectiblesWithLowestListingsFirst OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqListCollectiblesReturn> OnApiSuccess = [this, OnSuccess](const FSeqListCollectiblesReturn& Response)
	{
		OnSuccess.ExecuteIfBound(Response.CollectibleOrders, Response.Page);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Collectibles with Lowest Listings: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Marketplace->GetCollectiblesWithLowestListingsFirst(SequenceSdk::GetChainId(), ContractAddress, Filter, Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetAllCollectiblesWithLowestListingsFirst(const FString& ContractAddress, const FSeqCollectiblesFilter Filter, FOnGetAllCollectiblesWithLowestListingsFirst OnSuccess, FOnFailure OnFailure)
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
	
	this->Marketplace->GetAllCollectiblesWithLowestListingsFirst(SequenceSdk::GetChainId(), ContractAddress, Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetCollectiblesWithHighestPricedOffersFirst(const FString& ContractAddress, const FSeqCollectiblesFilter Filter, const FSeqMarketplacePage Page, FOnGetCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqListCollectiblesReturn> OnApiSuccess = [this, OnSuccess](const FSeqListCollectiblesReturn& Response)
	{
		OnSuccess.ExecuteIfBound(Response.CollectibleOrders, Response.Page);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Collectibles with Highest Priced Offers First: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Marketplace->GetCollectiblesWithHighestPricedOffersFirst(SequenceSdk::GetChainId(), ContractAddress, Filter, Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetAllCollectiblesWithHighestPricedOffersFirst(const FString& ContractAddress, const FSeqCollectiblesFilter Filter, FOnGetAllCollectiblesWithHighestPricedOffersFirst OnSuccess, FOnFailure OnFailure)
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
	
	this->Marketplace->ListAllCollectibleOffersWithHighestPricedOfferFirst(SequenceSdk::GetChainId(), ContractAddress, Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetLowestPriceOfferForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnGetLowestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnApiSuccess = [this, OnSuccess](const FSeqCollectibleOrder& Response)
	{
		OnSuccess.ExecuteIfBound(Response.TokenMetadata, Response.Order);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Lowest Price Offer for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetLowestPriceOfferForCollectible(SequenceSdk::GetChainId(), ContractAddress, TokenID, Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetHighestPriceOfferForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnGetHighestPriceOfferForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnApiSuccess = [this, OnSuccess](const FSeqCollectibleOrder& Response)
	{
		OnSuccess.ExecuteIfBound(Response.TokenMetadata, Response.Order);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Highest Price Offer for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetHighestPriceOfferForCollectible(SequenceSdk::GetChainId(), ContractAddress, TokenID, Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetLowestPriceListingForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnGetLowestPriceListingForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnApiSuccess = [this, OnSuccess](const FSeqCollectibleOrder& Response)
	{
		OnSuccess.ExecuteIfBound(Response.TokenMetadata, Response.Order);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Lowest Price Listing for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetLowestPriceListingForCollectible(SequenceSdk::GetChainId(), ContractAddress, TokenID, Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetHighestPriceListingForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnGetHighestPriceListingForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnApiSuccess = [this, OnSuccess](const FSeqCollectibleOrder& Response)
	{
		OnSuccess.ExecuteIfBound(Response.TokenMetadata, Response.Order);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Highest Price Listing for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetHighestPriceListingForCollectible(SequenceSdk::GetChainId(), ContractAddress, TokenID, Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListListingsForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, const FSeqMarketplacePage Page, FOnListListingsForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqListCollectibleListingsReturn> OnApiSuccess = [this, OnSuccess](const FSeqListCollectibleListingsReturn& Response)
	{
		OnSuccess.ExecuteIfBound(Response.Listings, Response.Page);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Listings for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->ListListingsForCollectible(SequenceSdk::GetChainId(), ContractAddress, TokenID, Filter, Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListAllListingsForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnListAllListingsForCollectible OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->ListAllListingsForCollectible(SequenceSdk::GetChainId(), ContractAddress, TokenID, Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListOffersForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, const FSeqMarketplacePage Page, FOnListOffersForCollectible OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqListCollectibleOffersReturn> OnApiSuccess = [this, OnSuccess](const FSeqListCollectibleOffersReturn& Response)
	{
		OnSuccess.ExecuteIfBound(Response.Offers, Response.Page);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Offers for Collectible: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->ListOffersForCollectible(SequenceSdk::GetChainId(), ContractAddress, TokenID, Filter, Page, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::ListAllOffersForCollectible(const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter Filter, FOnListAllOffersForCollectible OnSuccess, FOnFailure OnFailure)
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

	this->Marketplace->ListAllOffersForCollectible(SequenceSdk::GetChainId(), ContractAddress, TokenID, Filter, OnApiSuccess, OnApiFailure);
}

void USequenceMarketplaceBP::GetFloorOrder(const FString& ContractAddress, const FSeqCollectiblesFilter Filter, FOnGetFloorOrder OnSuccess, FOnFailure OnFailure)
{
	const TSuccessCallback<FSeqCollectibleOrder> OnApiSuccess = [this, OnSuccess](const FSeqCollectibleOrder& Response)
	{
		OnSuccess.ExecuteIfBound(Response.TokenMetadata, Response.Order);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Error getting Floor Order: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};

	this->Marketplace->GetFloorOrder(SequenceSdk::GetChainId(), ContractAddress, Filter, OnApiSuccess, OnApiFailure);
}
