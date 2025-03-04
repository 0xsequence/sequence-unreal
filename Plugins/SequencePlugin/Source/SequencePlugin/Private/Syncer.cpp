// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Syncer.h"

#include "Util/Log.h"

void USyncer::SetupForTesting(FString Name)
{
	this->SyncerName = Name;
}

void USyncer::Reset()
{
	this->Guard.Lock();
	this->RequestCount = 0;
	SEQ_LOG(Warning, TEXT("RESET Syncer: [%s] rCount: [%d]"), *this->SyncerName, this->RequestCount);
	this->OnDoneDelegate.ExecuteIfBound();//executes if bound!
	this->Guard.Unlock();
}

void USyncer::Increment()
{
	this->Guard.Lock();
	this->RequestCount++;
	SEQ_LOG(Warning, TEXT("INC Syncer: [%s] rCount: [%d]"), *this->SyncerName, this->RequestCount);
	this->Guard.Unlock();
}

void USyncer::Increase(int32 Amount)
{
	this->Guard.Lock();
	this->RequestCount += Amount;
	SEQ_LOG(Warning, TEXT("INC %d Syncer: [%s] rCount: [%d]"),Amount,*this->SyncerName, this->RequestCount);
	this->Guard.Unlock();
}

void USyncer::Decrement()
{
	this->Guard.Lock();
	this->RequestCount = FMath::Max(this->RequestCount-1, 0);//clamp to stop having values < 0
	SEQ_LOG(Warning, TEXT("DEC Syncer: [%s] rCount: [%d]"), *this->SyncerName, this->RequestCount);
	if (this->RequestCount == 0)
	{
		this->OnDoneDelegate.ExecuteIfBound();
	}
	this->Guard.Unlock();
}

void USyncer::Decrease(int32 Amount)
{
	this->Guard.Lock();
	this->RequestCount = FMath::Max(this->RequestCount - Amount, 0);//clamp to stop having values < 0
	SEQ_LOG(Warning, TEXT("DEC %d Syncer: [%s] rCount: [%d]"),Amount,*this->SyncerName, this->RequestCount);
	if (this->RequestCount == 0)
	{
		this->OnDoneDelegate.ExecuteIfBound();
	}
	this->Guard.Unlock();
}
