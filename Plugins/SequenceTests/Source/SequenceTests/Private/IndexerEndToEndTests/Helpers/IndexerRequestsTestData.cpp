// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "IndexerRequestsTestData.h"
#include "SequencePlugin/Public/Indexer/Indexer.h"

UIndexerRequestsTestData * UIndexerRequestsTestData::Make(const int32 PendingRequestsIn)
{
	UIndexerRequestsTestData * IndexerRequestsTestData = NewObject<UIndexerRequestsTestData>();
	IndexerRequestsTestData->PendingRequests = PendingRequestsIn;
	IndexerRequestsTestData->Indexer = NewObject<UIndexer>();
	return IndexerRequestsTestData;
}

int32 UIndexerRequestsTestData::DecrementPendingRequests()
{
	if (this->PendingRequests > 0)
	{
		this->PendingRequests -= 1;
		this->RequestsComplete += 1;
	}
	
	return this->PendingRequests;
}

int32 UIndexerRequestsTestData::GetPendingRequests() const
{
	return this->PendingRequests;
}

int32 UIndexerRequestsTestData::GetRequestsComplete() const
{
	return this->RequestsComplete;
}

void UIndexerRequestsTestData::RequestFailed()
{
	this->bAllRequestsSuccessful = false;
}

bool UIndexerRequestsTestData::GetAllRequestsSuccessful() const
{
	return this->bAllRequestsSuccessful;
}

bool UIndexerRequestsTestData::TestDone() const
{
	return this->PendingRequests == 0;
}

UIndexer * UIndexerRequestsTestData::GetIndexer() const
{
	return this->Indexer;
}
