// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Syncer.generated.h"

DECLARE_DELEGATE(FOnDoneSignature)//Delegate used to let others know when this syncer has zero active requests!

/**
 * Used to aid in the synchronization of asynchronous tasks
 */
UCLASS()
class SEQUENCEPLUGIN_API USyncer : public UObject
{
	GENERATED_BODY()
private:
	int32 RequestCount = 0;
	mutable FCriticalSection Guard;
	FString SyncerName;//used for testing!
public:

	FOnDoneSignature OnDoneDelegate;
	void SetupForTesting(FString Name);
	void Reset();//resets active request counter to zero and fires OnDone to clear everything out
	void Increment();
	void Increase(int32 Amount);
	void Decrement();
	void Decrease(int32 Amount);
	
};
