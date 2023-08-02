// Fill out your copyright notice in the Description page of Project Settings.


#include "IndexerTests.h"



void pingTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<bool> GenericSuccess = [OnSuccess](const bool bSuccess)
	{
		if (bSuccess)
		{
			OnSuccess("Ping Success");
		}
		else
		{
			OnSuccess("Ping Failure");
		}
	};

	const TFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("Ping Failure", Error);
	};

	indexer->Ping(testingChainID, GenericSuccess, GenericFailure);
}

void versionTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<FVersion> GenericSuccess = [OnSuccess](const FVersion version)
	{
		OnSuccess("Received Version Data");
		FString ret = UIndexerSupport::structToString<FVersion>(version);
		UE_LOG(LogTemp, Display, TEXT("Parsed Version Struct:\n%s\n"), *ret);
	};

	const TFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("Version Failure", Error);
	};

	indexer->Version(testingChainID, GenericSuccess, GenericFailure);
}

void runTimeStatusTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<FRuntimeStatus> GenericSuccess = [OnSuccess](const FRuntimeStatus runTimeStatus)
	{
		OnSuccess("Received RunTimeStatus Data");
		FString ret = UIndexerSupport::structToString<FRuntimeStatus>(runTimeStatus);
		UE_LOG(LogTemp, Display, TEXT("Parsed RunTimeStatus Struct:\n%s\n"), *ret);
	};

	const TFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("RunTimeStatus Failure", Error);
	};

	indexer->RunTimeStatus(testingChainID, GenericSuccess, GenericFailure);
}

void getChainIDTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<int64> GenericSuccess = [OnSuccess](const int64 chainID)
	{
		OnSuccess("Received ChainID Data");
		FString ret = FString::Printf(TEXT("%lld"),chainID);
		UE_LOG(LogTemp, Display, TEXT("ChainID: %s"), *ret);
	};

	const TFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("ChainID Failure", Error);
	};

	indexer->GetChainID(testingChainID, GenericSuccess, GenericFailure);
}

void getEtherBalanceTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<FEtherBalance> GenericSuccess = [OnSuccess](const FEtherBalance etherBalance)
	{
		OnSuccess("Received etherBalance Data");
		FString ret = UIndexerSupport::structToString<FEtherBalance>(etherBalance);
		UE_LOG(LogTemp, Display, TEXT("Parsed EtherBalance Struct:\n%s\n"), *ret);
	};

	const TFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("ChainID Failure", Error);
	};

	indexer->GetEtherBalance(testingChainID, testingAddress, GenericSuccess, GenericFailure);
}

void getTokenBalanceTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{

}

void getTokenSuppliesTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{

}

void getTokenSuppliesMapTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{

}

void getBalanceUpdatesTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{

}

void getTransactionHistoryTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{

}


void IndexerTest(TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	//now we need to test all the different calls!
	UIndexer* indexer = NewObject<UIndexer>();//for testing!	

	pingTest(indexer, OnSuccess, OnFailure);
	versionTest(indexer, OnSuccess, OnFailure);
	runTimeStatusTest(indexer, OnSuccess, OnFailure);
	getChainIDTest(indexer, OnSuccess, OnFailure);
	getEtherBalanceTest(indexer, OnSuccess, OnFailure);

	return;//done
}