// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Indexer.h"

/**
 * 
 */
const static FString GTestingAddress = "0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9";//peter's public address
const static FString GTestingContractAddress = "0xc58b7abe57605dee6355646aefd09c2dbfb68eb4";//contract address pulled from peter's data
const static int64 GTestingChainID = 137;//polygon for testing!
const static bool GPrintAll = true;//used to enable / disable printing of the received data for observation

//testing inputs
const static FString GTestingPage = "{\"page\":10,\"column\":\"left\",\"before\":\"b1\",\"after\":\"a1\",\"sort\":[{\"column\":\"left\",\"order\":\"DESC\"}],\"pageSize\":64,\"more\":true}";
const static FString GTestingContractInfo = "{\"chainId\":137,\"address\":\"0xa11\",\"name\":\"coin\",\"type\":\"t1\",\"symbol\":\"%\",\"decimals\":101,\"logoURI\":\"http://stuff.ca\",\"extensions\":{\"link\":\"https://that.com\",\"description\":\"extension\",\"ogImage\":\"uint8[]\",\"originChainId\":137,\"originAddress\":\"http://origin.ca\",\"blacklist\":true}}";
const static FString GTestingTokenMetaData = "{\"tokenId\":101,\"contractAddress\":\"0xc112\",\"name\":\"testing_name\",\"description\":\"some_desc_stuff\",\"image\":\"string_image_data\",\"decimals\":101,\"video\":\"video data\",\"audio\":\"audo_data\",\"image_data\":\"image_data\",\"external_url\":\"external_url_data\",\"background_color\":\"red\",\"animation_url\":\"http://anim.ca\",\"properties\":{\"p1\":10,\"p2\":\"prop_2\",\"p3\":1},\"attributes\":[{\"a11\":\"a\",\"a12\":\"b\"},{\"a21\":\"c\",\"a22\":\"d\",\"a23\":\"e\"}]}";
const static FString GTestingBalanceData = "{\"id\":10,\"contractAddress\":\"0xc10\",\"contractType\":\"ERC20\",\"accountAddress\":\"0xa10\",\"tokenID\":111,\"balance\":10101,\"blockHash\":\"0xb10\",\"blockNumber\":33,\"updateID\":66,\"chainId\":6,\"contractInfo\":"+GTestingContractInfo+",\"tokenMetaData\":"+GTestingTokenMetaData+"}";
const static FString GTestingTxnTransferData = "{\"transferType\":\"SEND\",\"contractAddress\":\"0xt1\",\"contractType\":\"ERC20\",\"from\":\"tester\",\"to\":\"testee\",\"tokenIds\":[0,1,2],\"amounts\":[12,13,14],\"logIndex\":1313,\"contractInfo\":"+GTestingContractInfo+",\"tokenMetaData\":{\"key1\":"+GTestingTokenMetaData+",\"key2\":"+GTestingTokenMetaData+"}}";
const static FString GTestingTransactionData = "{\"txnHash\":\"0xtxnHash\",\"blockNumber\":101313,\"blockHash\":\"0xBlockhash\",\"chainId\":1616,\"metaTxnID\":\"0xMetaTxnID\",\"transfers\":["+GTestingTxnTransferData+"],\"timestamp\":\"today\"}";
const static FString GTestTokenMap = "{\"keyOne\":[\"value1\",\"value2\"]}";
const static FString GTestHistoryFilterMax = "{\"accountAddress\":\"" + GTestingAddress + "\",\"contractAddress\":\"" + GTestingContractAddress + "\",\"accountAddresses\":[\"" + GTestingAddress + "\"],\"contractAddresses\":[\"" + GTestingContractAddress + "\"],\"transactionHashes\":[\"0x123abc\"],\"metaTransactionIDs\":[\"0xTEST\"],\"fromBlock\":10,\"toBlock\":101}";
const static FString GTestHistoryFilterMin = "{\"accountAddress\":\""+GTestingAddress+"\"}";
//testing inputs

//generators for testing//

/*
* The page here is mirror to
* FString testingPage and if any changes to FString testingPage occur
* the same changes need to be reflected here otherwise tests will fail!
*/
FPage BuildTestPage();

/*
* Builds a mirror TMap<FString,FTokenList> to FString testTokenMap
* for testing purposes, if anychanges occur to FString testTokenMap then
* matching changes must be made to this function in order to maintain
* test correctness
*/
TMap<FString, FTokenList> BuildTokenMap();

/*
* Builds a mirror to FString testHistoryFilter to testTransactionHistoryFull
* if any changes occur to FString testHistoryFilter then you must update this function to
*  match otherwise the test will fail due to a mismatch!
*/
FTransactionHistoryFilter BuildHistoryFilter();

//generators for testing//

//these are our general response tests used to test if we get back anything or not!
void PingTest(UIndexer *Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

void VersionTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

void RunTimeStatusTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

void GetChainIDTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

void GetEtherBalanceTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

void GetTokenBalanceTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

void GetTokenSuppliesTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

void GetTokenSuppliesMapTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

void GetBalanceUpdatesTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

void GetTransactionHistoryTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

void IndexerTest(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);

//response parsing tests
void EmptyResponseTest(UIndexer* Indexer);

void InvalidResponseTest(UIndexer* Indexer);

void WrongResponseReceivedTest1(UIndexer* Indexer);

void WrongResponseReceivedTest2(UIndexer* Indexer);

void PingParsingTest(UIndexer* Indexer);

void VersionParsingTest(UIndexer* Indexer);

void RunTimeStatusParsingTest(UIndexer* Indexer);

void GetChainIDParsingTest(UIndexer* Indexer);

void GetEtherBalanceParsingTest(UIndexer* Indexer);

void GetTokenBalanceParsingTest(UIndexer* Indexer);

void GetTokenSuppliesParsingTest(UIndexer* Indexer);

void GetTokenSuppliesMapParsingTest(UIndexer* Indexer);

void GetBalanceUpdatesParsingTest(UIndexer* Indexer);

void GetTransactionHistoryParsingTest(UIndexer* Indexer);

//arguement parsing tests
void TokenBalanceMaxArgsTest(UIndexer* Indexer);

void TokenBalanceMinArgsTest(UIndexer* Indexer);

void TokenSuppliesArgsTest(UIndexer* Indexer);

void TokenSuppliesMapArgsTest(UIndexer* Indexer);

void BalanceUpdatesMaxArgsTest(UIndexer* Indexer);

void BalanceUpdatesMinArgsTest(UIndexer* Indexer);

void TransactionHistoryArgsMaxTest(UIndexer* Indexer);

void TransactionHistoryArgsMinTest(UIndexer* Indexer);
