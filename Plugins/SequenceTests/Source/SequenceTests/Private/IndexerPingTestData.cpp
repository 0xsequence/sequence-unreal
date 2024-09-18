// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.


#include "IndexerPingTestData.h"
#include "SequencePlugin/Private/Indexer/Indexer.h"

UIndexerPingTestData * UIndexerPingTestData::Make(const int32 PendingPingsIn)
{
	UIndexerPingTestData * IndexerPingTestData = NewObject<UIndexerPingTestData>();
	IndexerPingTestData->PendingPings = PendingPingsIn;
	IndexerPingTestData->Indexer = NewObject<UIndexer>();
	return IndexerPingTestData;
}

int32 UIndexerPingTestData::DecrementPendingPings()
{
	if (this->PendingPings > 0)
	{
		this->PendingPings -= 1;
		this->PingsComplete += 1;
	}
	
	return this->PendingPings;
}

int32 UIndexerPingTestData::GetPendingPings() const
{
	return this->PendingPings;
}

int32 UIndexerPingTestData::GetPingsComplete() const
{
	return this->PingsComplete;
}

void UIndexerPingTestData::PingFailed()
{
	this->bAllPingsSuccessful = false;
}

bool UIndexerPingTestData::GetAllPingsSuccessful() const
{
	return this->bAllPingsSuccessful;
}

bool UIndexerPingTestData::TestDone() const
{
	return this->PendingPings == 0;
}

UIndexer * UIndexerPingTestData::GetIndexer() const
{
	return this->Indexer;
}
