// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IndexerPingTestData.generated.h"

/**
 * Used to track Async data for UIndexerPingTest
 */
UCLASS()
class UIndexerPingTestData : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	int32 PendingPings = 0;

	UPROPERTY()
	bool bAllPingsSuccessful = true;
	
public:
	
	/**
	 * Builds a UIndexerPingTestData UObject & Initializes it
	 * @param PendingPingsIn The PendingPing count we initialize with
	 * @return The Initialized UIndexerPingTestData UObject
	 */
	static UIndexerPingTestData * Make(const int32 PendingPingsIn);

	/**
	 * Decrements PendingPings & returns it
	 * @return The post decremented PendingPings int32
	 */
	int32 DecrementPendingPings();

	/**
	 * Returns PendingPings
	 * @return PendingPings
	 */
	int32 GetPendingPings() const;

	/**
	 * Sets bAllPingsSuccessful to false
	 */
	void PingFailed();

	bool TestDone() const;
};
