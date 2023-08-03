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
		OnFailure("etherBalance Failure", Error);
	};

	indexer->GetEtherBalance(testingChainID, testingAddress, GenericSuccess, GenericFailure);
}

void getTokenBalanceTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTokenBalancesReturn> GenericSuccess = [OnSuccess](const FGetTokenBalancesReturn tokenBalances)
	{
		OnSuccess("Received TokenBalances Data");
		FString ret = UIndexerSupport::structToString<FGetTokenBalancesReturn>(tokenBalances);
		UE_LOG(LogTemp, Display, TEXT("Parsed TokenBalancesReturn Struct:\n%s\n"), *ret);
	};

	const TFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("TokenBalances Failure", Error);
	};
	FGetTokenBalancesArgs args;
	args.accountAddress = testingAddress;
	indexer->GetTokenBalances(testingChainID, args, GenericSuccess, GenericFailure);
}

void getTokenSuppliesTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTokenSuppliesReturn> GenericSuccess = [OnSuccess](const FGetTokenSuppliesReturn tokenSupplies)
	{
		OnSuccess("Received TokenSupplies Data");
		FString ret = UIndexerSupport::structToString<FGetTokenSuppliesReturn>(tokenSupplies);
		UE_LOG(LogTemp, Display, TEXT("Parsed tokenSuppliesReturn Struct:\n%s\n"), *ret);
	};

	const TFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("TokenSupplies Failure", Error);
	};
	FGetTokenSuppliesArgs args;
	args.contractAddress = "0x8";
	args.includeMetaData = true;
	indexer->GetTokenSupplies(testingChainID, args, GenericSuccess, GenericFailure);
}

void getTokenSuppliesMapTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTokenSuppliesMapReturn> GenericSuccess = [OnSuccess](const FGetTokenSuppliesMapReturn tokenSuppliesMap)
	{
		OnSuccess("Received TokenSuppliesMap Data");
		FString ret = UIndexerSupport::structToString<FGetTokenSuppliesMapReturn>(tokenSuppliesMap);
		UE_LOG(LogTemp, Display, TEXT("Parsed TokenSuppliesMapReturn Struct:\n%s\n"), *ret);
	};

	const TFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("TokenSuppliesMap Failure", Error);
	};

	FGetTokenSuppliesMapArgs args;
	TMap<FString, FTokenList> tokenMap;
	args.includeMetaData = true;
	args.tokenMap = tokenMap;
	indexer->GetTokenSuppliesMap(testingChainID, args, GenericSuccess, GenericFailure);
}

void getBalanceUpdatesTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<FGetBalanceUpdatesReturn> GenericSuccess = [OnSuccess](const FGetBalanceUpdatesReturn balanceUpdates)
	{
		OnSuccess("Received balanceUpdates Data");
		FString ret = UIndexerSupport::structToString<FGetBalanceUpdatesReturn>(balanceUpdates);
		UE_LOG(LogTemp, Display, TEXT("Parsed balanceUpdatesReturn Struct:\n%s\n"), *ret);
	};

	const TFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("balanceUpdates Failure", Error);
	};

	FGetBalanceUpdatesArgs args;
	args.contractAddress = "0x";//need work here!
	indexer->GetBalanceUpdates(testingChainID, args, GenericSuccess, GenericFailure);
}

void getTransactionHistoryTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTransactionHistoryReturn> GenericSuccess = [OnSuccess](const FGetTransactionHistoryReturn transactionHistory)
	{
		OnSuccess("Received transactionHistory Data");
		FString ret = UIndexerSupport::structToString<FGetTransactionHistoryReturn>(transactionHistory);
		UE_LOG(LogTemp, Display, TEXT("Parsed transactionHistoryReturn Struct:\n%s\n"), *ret);
	};

	const TFailureCallback GenericFailure = [OnFailure](const SequenceError Error)
	{
		OnFailure("transactionHistory Failure", Error);
	};

	FGetTransactionHistoryArgs args;
	args.filter.accountAddress = testingAddress;
	args.includeMetaData = true;
	indexer->GetTransactionHistory(testingChainID, args, GenericSuccess, GenericFailure);
}


void IndexerTest(TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure)
{
	//now we need to test all the different calls!
	UIndexer* indexer = NewObject<UIndexer>();//for testing!	
	//these all work
//	pingTest(indexer, OnSuccess, OnFailure);
//	versionTest(indexer, OnSuccess, OnFailure);
//	runTimeStatusTest(indexer, OnSuccess, OnFailure);
//	getChainIDTest(indexer, OnSuccess, OnFailure);
//	getEtherBalanceTest(indexer, OnSuccess, OnFailure);

	//arg based indexer calls seem to be crashing for some reason could be bad arg handling?
	getTokenBalanceTest(indexer, OnSuccess, OnFailure);//*** exception thrown here! ***
	//getTokenSuppliesMapTest(indexer, OnSuccess, OnFailure);//*** exception thrown here! ***
	//getBalanceUpdatesTest(indexer, OnSuccess, OnFailure);//*** exception thrown here! ***
	//getTransactionHistoryTest(indexer, OnSuccess, OnFailure);//*** exception thrown here! ***

	return;//done
}