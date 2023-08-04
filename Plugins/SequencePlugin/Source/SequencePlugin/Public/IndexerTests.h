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

//testing inputs
const static FString testingPage = "{\"page\":10,\"column\":\"left\",\"before\":\"b1\",\"after\":\"a1\",\"sort\":[{\"column\":\"left\",\"order\":\"DESC\"}],\"pageSize\":64,\"more\":true}";
const static FString testingContractInfo = "{\"chainId\":137,\"address\":\"0xa11\",\"name\":\"coin\",\"type\":\"t1\",\"symbol\":\"%\",\"decimals\":101,\"logoURI\":\"http://stuff.ca\",\"extensions\":{\"link\":\"https://that.com\",\"description\":\"extension\",\"ogImage\":\"uint8[]\",\"originChainId\":137,\"originAddress\":\"http://origin.ca\",\"blacklist\":true}}";
const static FString testingTokenMetaData = "{\"tokenId\":101,\"contractAddress\":\"0xc112\",\"name\":\"testing_name\",\"description\":\"some_desc_stuff\",\"image\":\"string_image_data\",\"decimals\":101,\"video\":\"video data\",\"audio\":\"audo_data\",\"image_data\":\"image_data\",\"external_url\":\"external_url_data\",\"background_color\":\"red\",\"animation_url\":\"http://anim.ca\",\"properties\":{\"p1\":10,\"p2\":\"prop_2\",\"p3\":1},\"attributes\":[{\"a11\":\"a\",\"a12\":\"b\"},{\"a21\":\"c\",\"a22\":\"d\",\"a23\":\"e\"}]}";
const static FString testingBalanceData = "{\"id\":10,\"contractAddress\":\"0xc10\",\"contractType\":\"ERC20\",\"accountAddress\":\"0xa10\",\"tokenID\":111,\"balance\":10101,\"blockHash\":\"0xb10\",\"blockNumber\":33,\"updateID\":66,\"chainId\":6,\"contractInfo\":"+testingContractInfo+",\"tokenMetaData\":"+testingTokenMetaData+"}";
const static FString testingTxnTransferData = "{\"transferType\":\"SEND\",\"contractAddress\":\"0xt1\",\"contractType\":\"ERC20\",\"from\":\"tester\",\"to\":\"testee\",\"tokenIds\":[0,1,2],\"amounts\":[12,13,14],\"logIndex\":1313,\"contractInfo\":"+testingContractInfo+",\"tokenMetaData\":{\"key1\":"+testingTokenMetaData+",\"key2\":"+testingTokenMetaData+"}}";
const static FString testingTransactionData = "{\"txnHash\":\"0xtxnHash\",\"blockNumber\":101313,\"blockHash\":\"0xBlockhash\",\"chainId\":1616,\"metaTxnID\":\"0xMetaTxnID\",\"transfers\":["+testingTxnTransferData+"],\"timestamp\":\"today\"}";
const static FString testTokenMap = "{\"keyOne\":[\"value1\",\"value2\"]}";
//testing inputs

//generators for testing//

/*
* The page here is mirror to
* testingPage and if any changes to testingPage occur
* the same changes need to be reflected here otherwise tests will fail!
*/
FPage buildTestPage();

/*
* Builds a mirror TMap<FString,FTokenList> to testTokenMap
* for testing purposes, if anychanges occur to testTokenMap then
* matching changes must be made to this function in order to maintain
* test correctness
*/
TMap<FString, FTokenList> buildTokenMap();
//generators for testing//

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

//response parsing tests
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

//arguement parsing tests
void tokenBalanceArgsTest(UIndexer* indexer);

void tokenSuppliesArgsTest(UIndexer* indexer);

void tokenSuppliesMapArgsTest(UIndexer* indexer);

void balanceUpdatesArgsTest(UIndexer* indexer);

void transactionHistoryArgsTest(UIndexer* indexer);
