// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Syncer.generated.h"

DECLARE_DELEGATE(FOnDone)//Delegate used to let others know when this syncer has zero active requests!

/**
 * Used to aid in the synchronization of asynchronous tasks
 */
UCLASS()
class SEQUENCEPLUGIN_API USyncer : public UObject
{
	GENERATED_BODY()
private:
	int32 requestCount = 0;
public:
	void inc();
	void incN(int32 n);
	void dec();
	void decN(int32 n);
	
};
