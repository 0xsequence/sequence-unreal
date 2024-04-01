// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.


#include "Syncer.h"

void USyncer::SetupForTesting(FString Name)
{
	this->syncerName = Name;
}

void USyncer::Reset()
{
	this->Guard.Lock();
	this->requestCount = 0;
	UE_LOG(LogTemp, Warning, TEXT("RESET Syncer: [%s] rCount: [%d]"), *this->syncerName, this->requestCount);
	this->OnDoneDelegate.ExecuteIfBound();//executes if bound!
	this->Guard.Unlock();
}

void USyncer::Increment()
{
	this->Guard.Lock();
	this->requestCount++;
	UE_LOG(LogTemp, Warning, TEXT("INC Syncer: [%s] rCount: [%d]"), *this->syncerName, this->requestCount);
	this->Guard.Unlock();
}

void USyncer::Increase(int32 Amount)
{
	this->Guard.Lock();
	this->requestCount += Amount;
	UE_LOG(LogTemp, Warning, TEXT("INC %d Syncer: [%s] rCount: [%d]"),Amount,*this->syncerName, this->requestCount);
	this->Guard.Unlock();
}

void USyncer::Decrement()
{
	this->Guard.Lock();
	this->requestCount = FMath::Max(this->requestCount-1, 0);//clamp to stop having values < 0
	UE_LOG(LogTemp, Warning, TEXT("DEC Syncer: [%s] rCount: [%d]"), *this->syncerName, this->requestCount);
	if (this->requestCount == 0)
	{
		this->OnDoneDelegate.ExecuteIfBound();
	}
	this->Guard.Unlock();
}

void USyncer::Decrease(int32 Amount)
{
	this->Guard.Lock();
	this->requestCount = FMath::Max(this->requestCount - Amount, 0);//clamp to stop having values < 0
	UE_LOG(LogTemp, Warning, TEXT("DEC %d Syncer: [%s] rCount: [%d]"),Amount,*this->syncerName, this->requestCount);
	if (this->requestCount == 0)
	{
		this->OnDoneDelegate.ExecuteIfBound();
	}
	this->Guard.Unlock();
}
