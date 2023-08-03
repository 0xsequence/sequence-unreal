// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Indexer.h"

/**
 * 
 */
const static FString testingAddress = "0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9";//peter's public address
const static FString testingContractAddress = "0xc58b7abe57605dee6355646aefd09c2dbfb68eb4";//contract address pulled from peter's data
const static int64 testingChainID = 137;//polygon for testing!
const static bool printAll = true;//used to enable / disable printing of the received data for observation

//these are our general response tests used to test if we get back anything or not!
void pingTest(UIndexer * indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void versionTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void runTimeStatusTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void getChainIDTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void getEtherBalanceTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void getTokenBalanceTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void getTokenSuppliesTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void getTokenSuppliesMapTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void getBalanceUpdatesTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void getTransactionHistoryTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

void IndexerTest(TFunction<void(FString)> OnSuccess, TFunction<void(FString, SequenceError)> OnFailure);

//parser tests
void emptyResponseTest(UIndexer * indexer);

void invalidResponseTest(UIndexer * indexer);

void wrongResponseReceivedTest1(UIndexer * indexer);

void wrongResponseReceivedTest2(UIndexer* indexer);

void pingParsingTest(UIndexer* indexer);

void versionParsingTest(UIndexer* indexer);

void runTimeStatusParsingTest(UIndexer* indexer);

void getChainIDParsingTest(UIndexer* indexer);

void getEtherBalanceParsingTest(UIndexer* indexer);

void getTokenBalanceParsingTest(UIndexer* indexer);

void getTokenSuppliesParsingTest(UIndexer* indexer);

void getTokenSuppliesMapParsingTest(UIndexer* indexer);

void getBalanceUpdatesParsingTest(UIndexer* indexer);

void getTransactionHistoryParsingTest(UIndexer* indexer);

