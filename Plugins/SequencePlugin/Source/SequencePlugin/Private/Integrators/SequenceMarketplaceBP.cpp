#include "Integrators/SequenceMarketplaceBP.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Util/Log.h"

USequenceMarketplaceBP::USequenceMarketplaceBP()
{
	this->Marketplace = NewObject<UMarketplace>();

}

void USequenceMarketplaceBP::GetGetCollectiblesWithLowestListingsAsync(const int64 ChainId, const FSeqGetCollectiblesWithLowestListingsArgs& Args)
{
	const TSuccessCallback<FSeqGetCollectiblesWithLowestListingsReturn> OnSuccess = [this, ChainId, Args](const FSeqGetCollectiblesWithLowestListingsReturn& CollectiblesWithLowestListings)
		{
			this->CallCollectiblesWithLowestListingsReceived(true, ChainId, CollectiblesWithLowestListings);
		};

	const FFailureCallback OnFailure = [this, ChainId, Args](const FSequenceError& Error)
		{
			SEQ_LOG(Error, TEXT("Error getting Collectibles with Lowest Listings: %s"), *Error.Message);
			FSeqGetCollectiblesWithLowestListingsReturn CollectiblesWithLowestListings;
			this->CallCollectiblesWithLowestListingsReceived(false, ChainId, CollectiblesWithLowestListings);
		};


	this->Marketplace->GetCollectiblesWithLowestListings(ChainId, Args, OnSuccess, OnFailure);
}

void USequenceMarketplaceBP::CallCollectiblesWithLowestListingsReceived(const bool Status, const int64 ChainId, const FSeqGetCollectiblesWithLowestListingsReturn& Response)
{
	if (this->CollectiblesWithLowestListingsResponse.IsBound())
		this->CollectiblesWithLowestListingsResponse.Broadcast(Status, ChainId, Response);
	else
		SEQ_LOG(Error, TEXT("Nothing bound to delegate: CollectiblesWithLowestListingsResponse"));
}
