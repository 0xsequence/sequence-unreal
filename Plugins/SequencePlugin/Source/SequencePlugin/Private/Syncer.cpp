// Fill out your copyright notice in the Description page of Project Settings.


#include "Syncer.h"

void USyncer::reset()
{
	this->requestCount = 0;
	this->OnDoneDelegate.ExecuteIfBound();//executes if bound!
}

void USyncer::inc()
{
	this->requestCount++;
}

void USyncer::incN(int32 n)
{
	this->requestCount += n;
}

void USyncer::dec()
{
	this->requestCount = FMath::Max(this->requestCount-1, 0);//clamp to stop having values < 0
	if (this->requestCount == 0)
	{
		this->OnDoneDelegate.ExecuteIfBound();
	}
}

void USyncer::decN(int32 n)
{
	this->requestCount = FMath::Max(this->requestCount - n, 0);//clamp to stop having values < 0
	if (this->requestCount == 0)
	{
		this->OnDoneDelegate.ExecuteIfBound();
	}
}
