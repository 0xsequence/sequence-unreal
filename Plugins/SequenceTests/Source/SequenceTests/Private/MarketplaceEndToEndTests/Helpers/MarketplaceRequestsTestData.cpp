// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "MarketplaceRequestsTestData.h"
#include "SequencePlugin/Private/Indexer/Indexer.h"

UMarketplaceRequestsTestData * UMarketplaceRequestsTestData::Make(const int32 PendingRequestsIn)
{
	UMarketplaceRequestsTestData * IndexerRequestsTestData = NewObject<UMarketplaceRequestsTestData>();
	IndexerRequestsTestData->PendingRequests = PendingRequestsIn;
	IndexerRequestsTestData->Marketplace = NewObject<UMarketplace>();
	return IndexerRequestsTestData;
}

int32 UMarketplaceRequestsTestData::DecrementPendingRequests()
{
	if (this->PendingRequests > 0)
	{
		this->PendingRequests -= 1;
		this->RequestsComplete += 1;
	}
	
	return this->PendingRequests;
}

int32 UMarketplaceRequestsTestData::GetPendingRequests() const
{
	return this->PendingRequests;
}

int32 UMarketplaceRequestsTestData::GetRequestsComplete() const
{
	return this->RequestsComplete;
}

void UMarketplaceRequestsTestData::RequestFailed()
{
	this->bAllRequestsSuccessful = false;
}

bool UMarketplaceRequestsTestData::GetAllRequestsSuccessful() const
{
	return this->bAllRequestsSuccessful;
}

bool UMarketplaceRequestsTestData::TestDone() const
{
	return this->PendingRequests == 0;
}

UMarketplace * UMarketplaceRequestsTestData::GetMarketplace() const
{
	return this->Marketplace;
}
