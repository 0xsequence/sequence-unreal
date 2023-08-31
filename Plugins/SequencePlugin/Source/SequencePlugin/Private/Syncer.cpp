// Fill out your copyright notice in the Description page of Project Settings.


#include "Syncer.h"

void USyncer::setupForTesting(FString name)
{
	this->syncerName = name;
}

void USyncer::reset()
{
	this->Guard.Lock();
	this->requestCount = 0;
	UE_LOG(LogTemp, Warning, TEXT("RESET Syncer: [%s] rCount: [%d]"), *this->syncerName, this->requestCount);
	this->OnDoneDelegate.ExecuteIfBound();//executes if bound!
	this->Guard.Unlock();
}

void USyncer::inc()
{
	this->Guard.Lock();
	this->requestCount++;
	UE_LOG(LogTemp, Warning, TEXT("INC Syncer: [%s] rCount: [%d]"), *this->syncerName, this->requestCount);
	this->Guard.Unlock();
}

void USyncer::incN(int32 n)
{
	this->Guard.Lock();
	this->requestCount += n;
	UE_LOG(LogTemp, Warning, TEXT("INC %d Syncer: [%s] rCount: [%d]"),n,*this->syncerName, this->requestCount);
	this->Guard.Unlock();
}

void USyncer::dec()
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

void USyncer::decN(int32 n)
{
	this->Guard.Lock();
	this->requestCount = FMath::Max(this->requestCount - n, 0);//clamp to stop having values < 0
	UE_LOG(LogTemp, Warning, TEXT("DEC %d Syncer: [%s] rCount: [%d]"),n,*this->syncerName, this->requestCount);
	if (this->requestCount == 0)
	{
		this->OnDoneDelegate.ExecuteIfBound();
	}
	this->Guard.Unlock();
}
