// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SequencePlugin/Public/Marketplace/SequenceMarketplace.h"
#include "SequencePlugin/Public/Indexer/SequenceIndexer.h"
#include "MarketplaceRequestsTestData.generated.h"

/**
 * Used to track Async data for UIndexer tests that involve multiple chains
 */
UCLASS()
class UMarketplaceRequestsTestData : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY()
	int32 RequestsComplete = 0;
	
	UPROPERTY()
	int32 PendingRequests = 0;

	UPROPERTY()
	bool bAllRequestsSuccessful = true;

	UPROPERTY()
	USequenceMarketplace * Marketplace;
	
public:
	
	/**
	 * Builds a UIndexerRequestsTestData UObject & Initializes it
	 * @param PendingRequestsIn The PendingRequests count we initialize with
	 * @return The Initialized UIndexerVersionTestData UObject
	 */
	static UMarketplaceRequestsTestData * Make(const int32 PendingRequestsIn);

	/**
	 * Decrements PendingRequests & returns it, Also increments RequestsComplete
	 * @return The post decremented PendingRequests int32
	 */
	int32 DecrementPendingRequests();

	/**
	 * Returns PendingRequests
	 * @return PendingRequests
	 */
	int32 GetPendingRequests() const;

	/**
	 * Returns RequestsComplete
	 * @return RequestsComplete
	 */
	int32 GetRequestsComplete() const;

	/**
	 * Sets bAllRequestsSuccessful to false
	 */
	void RequestFailed();

	bool GetAllRequestsSuccessful() const;

	bool TestDone() const;

	USequenceMarketplace * GetMarketplace() const;
};
