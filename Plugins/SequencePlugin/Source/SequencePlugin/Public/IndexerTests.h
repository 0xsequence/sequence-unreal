// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Indexer.h"

/**
 * 
 */
const static FString testingAddress = "0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9";//peter's public address
const static int64 testingChainID = 137;//polygon for testing!

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
