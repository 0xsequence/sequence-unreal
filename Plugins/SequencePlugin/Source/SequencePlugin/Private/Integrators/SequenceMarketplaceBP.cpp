#include "Integrators/SequenceMarketplaceBP.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Util/Log.h"

USequenceMarketplaceBP::USequenceMarketplaceBP()
{
	this->Marketplace = NewObject<UMarketplace>();

}

void USequenceMarketplaceBP::GetGetCollectiblesWithLowestListingsAsync(const int64 ChainId, const FSeqListCollectiblesArgs& Args)
{
	const TSuccessCallback<FSeqListCollectiblesReturn> OnSuccess = [this, ChainId, Args](const FSeqListCollectiblesReturn& CollectiblesWithLowestListings)
		{
			this->CallCollectiblesWithLowestListingsReceived(true, ChainId, CollectiblesWithLowestListings);
		};

	const FFailureCallback OnFailure = [this, ChainId, Args](const FSequenceError& Error)
		{
			SEQ_LOG(Error, TEXT("Error getting Collectibles with Lowest Listings: %s"), *Error.Message);
			FSeqListCollectiblesReturn CollectiblesWithLowestListings;
			this->CallCollectiblesWithLowestListingsReceived(false, ChainId, CollectiblesWithLowestListings);
		};


	this->Marketplace->ListCollectibleListingsWithLowestPriceListingsFirst(ChainId, Args.ContractAddress, Args.Filter, Args.Page, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::CallCollectiblesWithLowestListingsReceived(const bool Status, const int64 ChainId, const FSeqListCollectiblesReturn& Response)
{
	if (this->CollectiblesWithLowestListingsResponse.IsBound())
		this->CollectiblesWithLowestListingsResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: CollectiblesWithLowestListingsResponse"));
}
