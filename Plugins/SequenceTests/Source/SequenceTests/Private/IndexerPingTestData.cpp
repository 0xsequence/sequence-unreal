// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.


#include "IndexerPingTestData.h"

UIndexerPingTestData * UIndexerPingTestData::Make(const int32 PendingPingsIn)
{
	UIndexerPingTestData * IndexerPingTestData = NewObject<UIndexerPingTestData>();
	IndexerPingTestData->PendingPings = PendingPingsIn;
	return IndexerPingTestData;
}

int32 UIndexerPingTestData::DecrementPendingPings()
{
	if (this->PendingPings > 0)
	{
		this->PendingPings -= 1;
	}
	else
	{
		this->PendingPings = 0;
	}
	
	return this->PendingPings;
}

int32 UIndexerPingTestData::GetPendingPings() const
{
	return this->PendingPings;
}

void UIndexerPingTestData::PingFailed()
{
	this->bAllPingsSuccessful = false;
}

bool UIndexerPingTestData::TestDone() const
{
	return this->PendingPings == 0;
}
