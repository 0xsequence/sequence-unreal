// Fill out your copyright notice in the Description page of Project Settings.

#include "Indexer/IndexerTests.h"

void pingTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
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

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("Ping Failure", Error);
	};

	indexer->Ping(testingChainID, GenericSuccess, GenericFailure);
}

void versionTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FVersion> GenericSuccess = [OnSuccess](const FVersion version)
	{
		OnSuccess("Received Version Data");
		if (printAll)
		{
			FString ret = UIndexerSupport::structToString<FVersion>(version);
			UE_LOG(LogTemp, Display, TEXT("Parsed Version Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("Version Failure", Error);
	};

	indexer->Version(testingChainID, GenericSuccess, GenericFailure);
}

void runTimeStatusTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FRuntimeStatus> GenericSuccess = [OnSuccess](const FRuntimeStatus runTimeStatus)
	{
		OnSuccess("Received RunTimeStatus Data");
		if (printAll)
		{
			FString ret = UIndexerSupport::structToString<FRuntimeStatus>(runTimeStatus);
			UE_LOG(LogTemp, Display, TEXT("Parsed RunTimeStatus Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("RunTimeStatus Failure", Error);
	};

	indexer->RunTimeStatus(testingChainID, GenericSuccess, GenericFailure);
}

void getChainIDTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<int64> GenericSuccess = [OnSuccess](const int64 chainID)
	{
		OnSuccess("Received ChainID Data");
		if (printAll)
		{
			FString ret = FString::Printf(TEXT("%lld"), chainID);
			UE_LOG(LogTemp, Display, TEXT("ChainID: %s"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("ChainID Failure", Error);
	};

	indexer->GetChainID(testingChainID, GenericSuccess, GenericFailure);
}

void getEtherBalanceTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FEtherBalance> GenericSuccess = [OnSuccess](const FEtherBalance etherBalance)
	{
		OnSuccess("Received etherBalance Data");
		if (printAll)
		{
			FString ret = UIndexerSupport::structToString<FEtherBalance>(etherBalance);
			UE_LOG(LogTemp, Display, TEXT("Parsed EtherBalance Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("etherBalance Failure", Error);
	};

	indexer->GetEtherBalance(testingChainID, testingAddress, GenericSuccess, GenericFailure);
}

void getTokenBalanceTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTokenBalancesReturn> GenericSuccess = [OnSuccess](const FGetTokenBalancesReturn tokenBalances)
	{
		OnSuccess("Received Token Balances Data");
		if (printAll)
		{
			FString ret = UIndexerSupport::structToString<FGetTokenBalancesReturn>(tokenBalances);
			UE_LOG(LogTemp, Display, TEXT("Parsed TokenBalancesReturn Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("Token Balances Failure", Error);
	};
	FGetTokenBalancesArgs args;
	args.accountAddress = testingAddress;
	args.includeMetaData = true;
	indexer->GetTokenBalances(testingChainID, args, GenericSuccess, GenericFailure);
}

void getTokenSuppliesTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTokenSuppliesReturn> GenericSuccess = [OnSuccess](const FGetTokenSuppliesReturn tokenSupplies)
	{
		OnSuccess("Received TokenSupplies Data");
		if (printAll)
		{
			FString ret = UIndexerSupport::structToString<FGetTokenSuppliesReturn>(tokenSupplies);
			UE_LOG(LogTemp, Display, TEXT("Parsed tokenSuppliesReturn Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("TokenSupplies Failure", Error);
	};
	FGetTokenSuppliesArgs args;
	args.contractAddress = testingContractAddress;
	args.includeMetaData = true;
	indexer->GetTokenSupplies(testingChainID, args, GenericSuccess, GenericFailure);
}

void getTokenSuppliesMapTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTokenSuppliesMapReturn> GenericSuccess = [OnSuccess](const FGetTokenSuppliesMapReturn tokenSuppliesMap)
	{
		OnSuccess("Received TokenSuppliesMap Data");
		if (printAll)
		{
			FString ret = UIndexerSupport::structToString<FGetTokenSuppliesMapReturn>(tokenSuppliesMap);
			UE_LOG(LogTemp, Display, TEXT("Parsed TokenSuppliesMapReturn Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("TokenSuppliesMap Failure", Error);
	};

	FGetTokenSuppliesMapArgs args;
	TMap<FString, FTokenList> tokenMap;
	TPair<FString,FTokenList> item;
	tokenMap.Add(item);
	args.includeMetaData = true;
	args.tokenMap = tokenMap;
	indexer->GetTokenSuppliesMap(testingChainID, args, GenericSuccess, GenericFailure);
}

void getBalanceUpdatesTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FGetBalanceUpdatesReturn> GenericSuccess = [OnSuccess](const FGetBalanceUpdatesReturn balanceUpdates)
	{
		OnSuccess("Received balance Updates Data");
		if (printAll)
		{
			FString ret = UIndexerSupport::structToString<FGetBalanceUpdatesReturn>(balanceUpdates);
			UE_LOG(LogTemp, Display, TEXT("Parsed balanceUpdatesReturn Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("balanceUpdates Failure", Error);
	};

	FGetBalanceUpdatesArgs args;
	args.contractAddress = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	args.page.page = 10;
	args.page.more = true;
	indexer->GetBalanceUpdates(testingChainID, args, GenericSuccess, GenericFailure);
}

void getTransactionHistoryTest(UIndexer* indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTransactionHistoryReturn> GenericSuccess = [OnSuccess](const FGetTransactionHistoryReturn transactionHistory)
	{
		OnSuccess("Received transactionHistory Data");
		if (printAll)
		{
			FString ret = UIndexerSupport::structToString<FGetTransactionHistoryReturn>(transactionHistory);
			UE_LOG(LogTemp, Display, TEXT("Parsed transactionHistoryReturn Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("transactionHistory Failure", Error);
	};

	FGetTransactionHistoryArgs args;
	args.filter.accountAddress = "0x0E0f9d1c4BeF9f0B8a2D9D4c09529F260C7758A2";
	args.includeMetaData = true;
	args.page.page = 0;
	args.page.more = true;
	indexer->GetTransactionHistory(testingChainID, args, GenericSuccess, GenericFailure);
}

void emptyResponseTest(UIndexer * indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("EmptyResponseTest"));
	FString args = "{}";
	FPingReturn rep = indexer->BuildResponse<FPingReturn>(args);
	//by default this should still be false!
	if (!rep.status)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void invalidResponseTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Invalid Response Test"));
	FString args = "+)0asfjlkj;ksjfa{sdfkaljsa;f[}}}skaflasjfd;";
	FPingReturn rep = indexer->BuildResponse<FPingReturn>(args);
	//by default this should still be false!
	if (!rep.status)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void wrongResponseReceivedTest1(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Wrong Response Received Test 1"));
	FString args = "{\"wrong_arg\" : true}";
	FPingReturn rep = indexer->BuildResponse<FPingReturn>(args);
	//by default this should still be false!
	if (!rep.status)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void wrongResponseReceivedTest2(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Wrong Response Received Test 2"));
	FString args = "{ [ \"alt1\" , \"alt2\" , \"alt3\" ] }";
	FPingReturn rep = indexer->BuildResponse<FPingReturn>(args);
	//by default this should still be false!
	if (!rep.status)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

//end of return parsing tests
void pingParsingTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Ping Parsing Test"));
	FString args = "{\"status\":true}";
	FPingReturn rep = indexer->BuildResponse<FPingReturn>(args);
	args = UIndexerSupport::simplifyString(args);

	//convert response to string testable string format
	FString repString = UIndexerSupport::structToSimpleString<FPingReturn>(rep);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"),*args);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"),*repString);
	}

	if (args.ToLower().Compare(repString.ToLower())==0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void versionParsingTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Version Parsing Test"));
	FString args = "{\"version\":{\"webrpcVersion\":\"1.0\" , \"schemaVersion\":\"s1.0\" , \"schemaHash\":\"0x1\" , \"appVersion\":\"a1.0\"}}";
	FVersionReturn rep = indexer->BuildResponse<FVersionReturn>(args);
	args = UIndexerSupport::simplifyString(args);

	//convert response to string testable string format
	FString repString = UIndexerSupport::structToSimpleString<FVersionReturn>(rep);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *args);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *repString);
	}

	if (args.ToLower().Compare(repString.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void runTimeStatusParsingTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Runtime Status Parsing Test"));
	FString args = "{\"status\":{\"healthOK\":true,\"indexerEnabled\":true,\"startTime\":\"now\",\"uptime\":10.1,\"ver\":\"1.1\",\"branch\":\"main\",\"commitHash\":\"0x12\",\"chainID\":1,\"checks\":{\"running\":true,\"syncMode\":\"Synced\",\"lastBlockNum\":10}}}";
	FRuntimeStatusReturn rep = indexer->BuildResponse<FRuntimeStatusReturn>(args);
	args = UIndexerSupport::simplifyString(args);

	//convert response to string testable string format
	FString repString = UIndexerSupport::structToSimpleString<FRuntimeStatusReturn>(rep);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *args);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *repString);
	}

	if (args.ToLower().Compare(repString.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void getChainIDParsingTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Chain ID Parsing Test"));
	FString args = "{\"chainID\":169}";
	FGetChainIDReturn rep = indexer->BuildResponse<FGetChainIDReturn>(args);
	args = UIndexerSupport::simplifyString(args);

	//convert response to string testable string format
	FString repString = UIndexerSupport::structToSimpleString<FGetChainIDReturn>(rep);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *args);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *repString);
	}

	if (args.ToLower().Compare(repString.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void getEtherBalanceParsingTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Ether Balance Parsing Test"));
	FString args = "{\"balance\":{\"accountAddress\":\"main_address\",\"balanceWei\":101}}";
	FGetEtherBalanceReturn rep = indexer->BuildResponse<FGetEtherBalanceReturn>(args);
	args = UIndexerSupport::simplifyString(args);

	//convert response to string testable string format
	FString repString = UIndexerSupport::structToSimpleString<FGetEtherBalanceReturn>(rep);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *args);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *repString);
	}

	if (args.ToLower().Compare(repString.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void getTokenBalanceParsingTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Token Balance Parsing Test"));
	FString args = "{\"page\":{\"page\":10,\"column\":\"left\",\"before\":\"b1\",\"after\":\"a1\",\"sort\":[{\"column\":\"left\",\"order\":\"DESC\"}],\"pageSize\":64,\"more\":true},\"balances\":[{\"id\":15,\"contractAddress\":\"0xc1\",\"contractType\":\"ERC20\",\"accountAddress\":\"0xa1\",\"tokenID\":11,\"balance\":16384,\"blockHash\":\"0xb1\",\"blockNumber\":1,\"updateID\":3,\"chainId\":137,\"contractInfo\":{\"chainId\":137,\"address\":\"0xa11\",\"name\":\"coin\",\"type\":\"t1\",\"symbol\":\"%\",\"decimals\":101,\"logoURI\":\"http://stuff.ca\",\"extensions\":{\"link\":\"https://that.com\",\"description\":\"extension\",\"ogImage\":\"uint8[]\",\"originChainId\":137,\"originAddress\":\"http://origin.ca\",\"blacklist\":true}},\"tokenMetaData\":{\"tokenId\":101,\"contractAddress\":\"0xc112\",\"name\":\"testing_name\",\"description\":\"some_desc_stuff\",\"image\":\"string_image_data\",\"decimals\":101,\"video\":\"video data\",\"audio\":\"audo_data\",\"image_data\":\"image_data\",\"external_url\":\"external_url_data\",\"background_color\":\"red\",\"animation_url\":\"http://anim.ca\",\"properties\":{\"p1\":10,\"p2\":\"prop_2\",\"p3\":1},\"attributes\":[{\"a11\":\"a\",\"a12\":\"b\"},{\"a21\":\"c\",\"a22\":\"d\",\"a23\":\"e\"}]}}]}";
	FGetTokenBalancesReturn rep = indexer->BuildResponse<FGetTokenBalancesReturn>(args);
	args = UIndexerSupport::simplifyString(args);

	//convert response to string testable string format
	//because this objects uses custom setup and parsing we must use custom getting to test with it! unreal won't be able 
	//to parse it completely
	FString repString = UIndexerSupport::jsonToSimpleString(rep.GetJson());

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *args);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *repString);
	}

	if (args.ToLower().Compare(repString.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void getTokenSuppliesParsingTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("get Token Supplies Parsing Test"));
	FString args = "{\"page\":{\"page\":10,\"column\":\"left\",\"before\":\"b1\",\"after\":\"a1\",\"sort\":[{\"column\":\"left\",\"order\":\"DESC\"}],\"pageSize\":64,\"more\":true},\"contractType\":\"ERC1155\",\"tokenIDs\":[{\"tokenID\":69,\"supply\":\"supply_data\",\"chainId\":9,\"contractInfo\":{\"chainId\":137,\"address\":\"0xa11\",\"name\":\"coin\",\"type\":\"t1\",\"symbol\":\"%\",\"decimals\":101,\"logoURI\":\"http://stuff.ca\",\"extensions\":{\"link\":\"https://that.com\",\"description\":\"extension\",\"ogImage\":\"uint8[]\",\"originChainId\":137,\"originAddress\":\"http://origin.ca\",\"blacklist\":true}},\"tokenMetaData\":{\"tokenId\":101,\"contractAddress\":\"0xc112\",\"name\":\"testing_name\",\"description\":\"some_desc_stuff\",\"image\":\"string_image_data\",\"decimals\":101,\"video\":\"video data\",\"audio\":\"audo_data\",\"image_data\":\"image_data\",\"external_url\":\"external_url_data\",\"background_color\":\"red\",\"animation_url\":\"http://anim.ca\",\"properties\":{\"p1\":10,\"p2\":\"prop_2\",\"p3\":1},\"attributes\":[{\"a11\":\"a\",\"a12\":\"b\"},{\"a21\":\"c\",\"a22\":\"d\",\"a23\":\"e\"}]}}]}";
	FGetTokenSuppliesReturn rep = indexer->BuildResponse<FGetTokenSuppliesReturn>(args);
	args = UIndexerSupport::simplifyString(args);

	//convert response to string testable string format
	//because this objects uses custom setup and parsing we must use custom getting to test with it! unreal won't be able 
	//to parse it completely
	FString repString = UIndexerSupport::jsonToSimpleString(rep.GetJson());

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *args);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *repString);
	}

	if (args.ToLower().Compare(repString.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void getTokenSuppliesMapParsingTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("get Token Supplies Map Parsing Test"));
	FString args = "{\"supplies\":{\"key1\":[{\"tokenID\":101,\"supply\":\"supply_data\",\"chainId\":137,\"contractInfo\":{\"chainId\":137,\"address\":\"0xa11\",\"name\":\"coin\",\"type\":\"t1\",\"symbol\":\"%\",\"decimals\":101,\"logoURI\":\"http://stuff.ca\",\"extensions\":{\"link\":\"https://that.com\",\"description\":\"extension\",\"ogImage\":\"uint8[]\",\"originChainId\":137,\"originAddress\":\"http://origin.ca\",\"blacklist\":true}},\"tokenMetaData\":{\"tokenId\":101,\"contractAddress\":\"0xc112\",\"name\":\"testing_name\",\"description\":\"some_desc_stuff\",\"image\":\"string_image_data\",\"decimals\":101,\"video\":\"video data\",\"audio\":\"audo_data\",\"image_data\":\"image_data\",\"external_url\":\"external_url_data\",\"background_color\":\"red\",\"animation_url\":\"http://anim.ca\",\"properties\":{\"p1\":10,\"p2\":\"prop_2\",\"p3\":1},\"attributes\":[{\"a11\":\"a\",\"a12\":\"b\"},{\"a21\":\"c\",\"a22\":\"d\",\"a23\":\"e\"}]}}]}}";
	FGetTokenSuppliesMapReturn rep = indexer->BuildResponse<FGetTokenSuppliesMapReturn>(args);
	args = UIndexerSupport::simplifyString(args);

	//convert response to string testable string format
	//because this objects uses custom setup and parsing we must use custom getting to test with it! unreal won't be able 
	//to parse it completely
	FString repString = UIndexerSupport::jsonToSimpleString(rep.GetJson());

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *args);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *repString);
	}

	if (args.ToLower().Compare(repString.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void getBalanceUpdatesParsingTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("get Balance Updates Parsing Test"));
	FString args = "{\"page\":"+testingPage+",\"balances\":["+testingBalanceData+"]}";
	FGetBalanceUpdatesReturn rep = indexer->BuildResponse<FGetBalanceUpdatesReturn>(args);
	args = UIndexerSupport::simplifyString(args);

	//convert response to string testable string format
	//because this objects uses custom setup and parsing we must use custom getting to test with it! unreal won't be able 
	//to parse it completely
	FString repString = UIndexerSupport::jsonToSimpleString(rep.GetJson());

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *args);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *repString);
	}

	if (args.ToLower().Compare(repString.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void getTransactionHistoryParsingTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("get Transaction History Parsing Test"));
	FString args = "{\"page\":"+testingPage+",\"transactions\":["+testingTransactionData+"]}";
	FGetTransactionHistoryReturn rep = indexer->BuildResponse<FGetTransactionHistoryReturn>(args);
	args = UIndexerSupport::simplifyString(args);

	//convert response to string testable string format
	//because this objects uses custom setup and parsing we must use custom getting to test with it! unreal won't be able 
	//to parse it completely
	FString repString = UIndexerSupport::jsonToSimpleString(rep.GetJson());

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *args);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *repString);
	}

	if (args.ToLower().Compare(repString.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}
//end of return parsing tests

//start of args parsing tests
void tokenBalanceMaxArgsTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Token Balance [MAX] Args Parsing Test"));
	FString testArgs = "{\"accountAddress\":\""+testingAddress+"\",\"contractAddress\":\""+testingContractAddress+"\",\"includeMetaData\":true,\"page\":"+testingPage+"}";
	
	FGetTokenBalancesArgs args;
	args.accountAddress = testingAddress;
	args.contractAddress = testingContractAddress;
	args.includeMetaData = true;
	args.page = buildTestPage();
	
	FString stringArgs = UIndexerSupport::simplifyStringParsable(indexer->BuildArgs<FGetTokenBalancesArgs>(args));
	testArgs = UIndexerSupport::simplifyStringParsable(testArgs);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *testArgs);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *stringArgs);
	}

	if (stringArgs.ToLower().Compare(testArgs.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void tokenBalanceMinArgsTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Token Balance [MIN] Args Parsing Test"));
	FString testArgs = "{\"accountAddress\":\"" + testingAddress + "\",\"includeMetaData\":false}";

	FGetTokenBalancesArgs args;
	args.accountAddress = testingAddress;

	FString stringArgs = UIndexerSupport::simplifyStringParsable(indexer->BuildArgs<FGetTokenBalancesArgs>(args));
	testArgs = UIndexerSupport::simplifyStringParsable(testArgs);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *testArgs);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *stringArgs);
	}

	if (stringArgs.ToLower().Compare(testArgs.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

/*
* The page here is mirror to
* testingPage and if any changes to testingPage occur
* the same changes need to be reflected here otherwise tests will fail!
*/
FPage buildTestPage()
{
	//setup page data!
	FPage ret;
	ret.page = 10;
	ret.column = "left";
	ret.before = "b1";
	ret.after = "a1";

	TArray<FSortBy> sortList;
	FSortBy sortByEntry;
	sortByEntry.column = "left";
	sortByEntry.order = ESortOrder_Sequence::DESC;
	sortList.Add(sortByEntry);

	ret.sort = sortList;
	ret.pageSize = 64;
	ret.more = true;
	//setup page data!
	return ret;
}

TMap<FString, FTokenList> buildTokenMap()
{
	TMap<FString, FTokenList> ret;
	//{\"keyOne\":[\"value1\",\"value2\"]}
	FTokenList rItem;//array of strings!
	rItem.token_list.Add("value1");
	rItem.token_list.Add("value2");
	ret.Add(TPair<FString,FTokenList>("keyOne",rItem));
	return ret;
}

FTransactionHistoryFilter buildHistoryFilter()
{
	FTransactionHistoryFilter ret;
	ret.accountAddress = testingAddress;
	ret.contractAddress = testingContractAddress;
	ret.accountAddresses.Add(testingAddress);
	ret.contractAddresses.Add(testingContractAddress);
	ret.transactionHashes.Add("0x123abc");
	ret.metaTransactionIDs.Add("0xTEST");
	ret.fromBlock = 10;
	ret.toBlock = 101;
	return ret;
}

void tokenSuppliesArgsTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Token Supplies Args Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"contractAddress\":\""+testingContractAddress+"\",\"includeMetaData\":true,\"page\":"+testingPage+"}";

	FGetTokenSuppliesArgs args;
	args.contractAddress = testingContractAddress;
	args.includeMetaData = true;
	args.page = buildTestPage();

	FString stringArgs = UIndexerSupport::simplifyStringParsable(indexer->BuildArgs<FGetTokenSuppliesArgs>(args));
	testArgs = UIndexerSupport::simplifyStringParsable(testArgs);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *testArgs);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *stringArgs);
	}

	if (stringArgs.ToLower().Compare(testArgs.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void tokenSuppliesMapArgsTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Token Supplies Map Args Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"tokenMap\":"+testTokenMap+",\"includeMetaData\":true}";
	FGetTokenSuppliesMapArgs args;
	args.tokenMap = buildTokenMap();
	args.includeMetaData = true;
	FString stringArgs = UIndexerSupport::simplifyStringParsable(indexer->BuildArgs<FGetTokenSuppliesMapArgs>(args));
	testArgs = UIndexerSupport::simplifyStringParsable(testArgs);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *testArgs);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *stringArgs);
	}

	if (stringArgs.ToLower().Compare(testArgs.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void balanceUpdatesMaxArgsTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("balance Updates [MAX] Args Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"contractAddress\":\""+testingContractAddress+"\",\"lastUpdateID\":10,\"page\":"+testingPage+"}";
	FGetBalanceUpdatesArgs args;
	args.contractAddress = testingContractAddress;
	args.lastUpdateID = 10;
	args.page = buildTestPage();

	FString stringArgs = UIndexerSupport::simplifyStringParsable(indexer->BuildArgs<FGetBalanceUpdatesArgs>(args));
	testArgs = UIndexerSupport::simplifyStringParsable(testArgs);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *testArgs);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *stringArgs);
	}

	if (stringArgs.ToLower().Compare(testArgs.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void balanceUpdatesMinArgsTest(UIndexer * indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("balance Updates [MIN] Args Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"contractAddress\":\"" + testingContractAddress + "\"}";
	FGetBalanceUpdatesArgs args;
	args.contractAddress = testingContractAddress;

	FString stringArgs = UIndexerSupport::simplifyStringParsable(indexer->BuildArgs<FGetBalanceUpdatesArgs>(args));
	testArgs = UIndexerSupport::simplifyStringParsable(testArgs);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *testArgs);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *stringArgs);
	}

	if (stringArgs.ToLower().Compare(testArgs.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

//set of args testing for transaction history filter as there can be many types of calls made!

void transactionHistoryArgsMaxTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("transaction History Args [Max] Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"filter\":"+ testHistoryFilterMax +",\"page\":"+testingPage+",\"includeMetaData\":true}";
	FGetTransactionHistoryArgs args;
	args.filter = buildHistoryFilter();
	args.page = buildTestPage();
	args.includeMetaData = true;

	FString stringArgs = UIndexerSupport::simplifyStringParsable(indexer->BuildArgs<FGetTransactionHistoryArgs>(args));
	testArgs = UIndexerSupport::simplifyStringParsable(testArgs);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *testArgs);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *stringArgs);
	}

	if (stringArgs.ToLower().Compare(testArgs.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void transactionHistoryArgsMinTest(UIndexer* indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("transaction History Args [Min] Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"filter\":" + testHistoryFilterMin + ",\"includeMetaData\":true}";
	FGetTransactionHistoryArgs args;
	args.filter.accountAddress = testingAddress;
	args.includeMetaData = true;

	FString stringArgs = UIndexerSupport::simplifyStringParsable(indexer->BuildArgs<FGetTransactionHistoryArgs>(args));
	testArgs = UIndexerSupport::simplifyStringParsable(testArgs);

	if (printAll)
	{
		UE_LOG(LogTemp, Display, TEXT("In:\n%s"), *testArgs);
		UE_LOG(LogTemp, Display, TEXT("Out:\n%s"), *stringArgs);
	}

	if (stringArgs.ToLower().Compare(testArgs.ToLower()) == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Passed"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed"));
	}
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
}

void IndexerTest(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	//now we need to test all the different calls!
	UIndexer* indexer = NewObject<UIndexer>();//for testing!

	//failure testing//
	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));
	UE_LOG(LogTemp, Display, TEXT("Start of Failure Tests"));
	emptyResponseTest(indexer);
	invalidResponseTest(indexer);
	wrongResponseReceivedTest1(indexer);
	wrongResponseReceivedTest2(indexer);
	UE_LOG(LogTemp, Display, TEXT("End of Failure Tests"));
	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));

	//parsing tests//
	//buildResponse parsing tests//
	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));
	UE_LOG(LogTemp, Display, TEXT("Start of Response Tests"));
	pingParsingTest(indexer);
	versionParsingTest(indexer);
	runTimeStatusParsingTest(indexer);
	getChainIDParsingTest(indexer);
	getEtherBalanceParsingTest(indexer);
	getTokenBalanceParsingTest(indexer);
	getTokenSuppliesParsingTest(indexer);
	getTokenSuppliesMapParsingTest(indexer);
	getBalanceUpdatesParsingTest(indexer);
	getTransactionHistoryParsingTest(indexer);
	//buildResponse parsing tests//
	UE_LOG(LogTemp, Display, TEXT("End of Response Tests"));
	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));

	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));
	UE_LOG(LogTemp, Display, TEXT("Start of Argument Tests"));
	//buildArgs parsing tests//
	tokenBalanceMaxArgsTest(indexer);
	tokenBalanceMinArgsTest(indexer);
	tokenSuppliesArgsTest(indexer);
	tokenSuppliesMapArgsTest(indexer);
	balanceUpdatesMaxArgsTest(indexer);
	balanceUpdatesMinArgsTest(indexer);
	transactionHistoryArgsMaxTest(indexer);
	transactionHistoryArgsMinTest(indexer);
	//buildArgs parsing tests//
	UE_LOG(LogTemp, Display, TEXT("End of Argument Tests"));
	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));

	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));
	UE_LOG(LogTemp, Display, TEXT("Start of System Tests"));
	//system tests//
	pingTest(indexer, OnSuccess, OnFailure);
	versionTest(indexer, OnSuccess, OnFailure);
	runTimeStatusTest(indexer, OnSuccess, OnFailure);
	getChainIDTest(indexer, OnSuccess, OnFailure);
	getEtherBalanceTest(indexer, OnSuccess, OnFailure);
	getTokenBalanceTest(indexer, OnSuccess, OnFailure);
	getTokenSuppliesTest(indexer,OnSuccess, OnFailure);
	getTokenSuppliesMapTest(indexer, OnSuccess, OnFailure);
	getBalanceUpdatesTest(indexer, OnSuccess, OnFailure);
	getTransactionHistoryTest(indexer, OnSuccess, OnFailure);
	return;//done
}