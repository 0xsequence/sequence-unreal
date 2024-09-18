// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IndexerPingTestData.generated.h"

class UIndexer;

/**
 * Used to track Async data for UIndexerPingTest
 */
UCLASS()
class UIndexerPingTestData : public UObject
{
	GENERATED_BODY()

	/**
	 * This variable is the inverse of PendingPings, IE)
	 * If PendingPings is 27 then this is 0, Each time PendingPings is decremented
	 * PingsComplete is incremented by the time PendingPings is 0, PingsComplete will be what
	 * PendingPings initial value was
	 */
	UPROPERTY()
	int32 PingsComplete = 0;
	
	UPROPERTY()
	int32 PendingPings = 0;

	UPROPERTY()
	bool bAllPingsSuccessful = true;

	UPROPERTY()
	UIndexer * Indexer;
	
public:
	
	/**
	 * Builds a UIndexerPingTestData UObject & Initializes it
	 * @param PendingPingsIn The PendingPing count we initialize with
	 * @return The Initialized UIndexerPingTestData UObject
	 */
	static UIndexerPingTestData * Make(const int32 PendingPingsIn);

	/**
	 * Decrements PendingPings & returns it, Also increments PingsComplete
	 * @return The post decremented PendingPings int32
	 */
	int32 DecrementPendingPings();

	/**
	 * Returns PendingPings
	 * @return PendingPings
	 */
	int32 GetPendingPings() const;

	/**
	 * Returns PingsComplete
	 * @return PingsComplete
	 */
	int32 GetPingsComplete() const;

	/**
	 * Sets bAllPingsSuccessful to false
	 */
	void PingFailed();

	bool GetAllPingsSuccessful() const;

	bool TestDone() const;

	UIndexer * GetIndexer() const;
};
