// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Indexer/IndexerTests.h"

void PingTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
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

	Indexer->Ping(GTestingChainID, GenericSuccess, GenericFailure);
}

void VersionTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FVersion> GenericSuccess = [OnSuccess](const FVersion version)
	{
		OnSuccess("Received Version Data");
		if (GPrintAll)
		{
			FString ret = UIndexerSupport::StructToString<FVersion>(version);
			UE_LOG(LogTemp, Display, TEXT("Parsed Version Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("Version Failure", Error);
	};

	Indexer->Version(GTestingChainID, GenericSuccess, GenericFailure);
}

void RunTimeStatusTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FRuntimeStatus> GenericSuccess = [OnSuccess](const FRuntimeStatus runTimeStatus)
	{
		OnSuccess("Received RunTimeStatus Data");
		if (GPrintAll)
		{
			FString ret = UIndexerSupport::StructToString<FRuntimeStatus>(runTimeStatus);
			UE_LOG(LogTemp, Display, TEXT("Parsed RunTimeStatus Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("RunTimeStatus Failure", Error);
	};

	Indexer->RunTimeStatus(GTestingChainID, GenericSuccess, GenericFailure);
}

void GetChainIDTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<int64> GenericSuccess = [OnSuccess](const int64 chainID)
	{
		OnSuccess("Received ChainID Data");
		if (GPrintAll)
		{
			FString ret = FString::Printf(TEXT("%lld"), chainID);
			UE_LOG(LogTemp, Display, TEXT("ChainID: %s"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("ChainID Failure", Error);
	};

	Indexer->GetChainID(GTestingChainID, GenericSuccess, GenericFailure);
}

void GetEtherBalanceTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FEtherBalance> GenericSuccess = [OnSuccess](const FEtherBalance etherBalance)
	{
		OnSuccess("Received etherBalance Data");
		if (GPrintAll)
		{
			FString ret = UIndexerSupport::StructToString<FEtherBalance>(etherBalance);
			UE_LOG(LogTemp, Display, TEXT("Parsed EtherBalance Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("etherBalance Failure", Error);
	};

	Indexer->GetEtherBalance(GTestingChainID, GTestingAddress, GenericSuccess, GenericFailure);
}

void GetTokenBalanceTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTokenBalancesReturn> GenericSuccess = [OnSuccess](const FGetTokenBalancesReturn tokenBalances)
	{
		OnSuccess("Received Token Balances Data");
		if (GPrintAll)
		{
			FString ret = UIndexerSupport::StructToString<FGetTokenBalancesReturn>(tokenBalances);
			UE_LOG(LogTemp, Display, TEXT("Parsed TokenBalancesReturn Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("Token Balances Failure", Error);
	};
	FGetTokenBalancesArgs args;
	args.accountAddress = GTestingAddress;
	args.includeMetaData = true;
	Indexer->GetTokenBalances(GTestingChainID, args, GenericSuccess, GenericFailure);
}

void GetTokenSuppliesTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTokenSuppliesReturn> GenericSuccess = [OnSuccess](const FGetTokenSuppliesReturn tokenSupplies)
	{
		OnSuccess("Received TokenSupplies Data");
		if (GPrintAll)
		{
			FString ret = UIndexerSupport::StructToString<FGetTokenSuppliesReturn>(tokenSupplies);
			UE_LOG(LogTemp, Display, TEXT("Parsed tokenSuppliesReturn Struct:\n%s\n"), *ret);
		}
	};

	const FFailureCallback GenericFailure = [OnFailure](const FSequenceError Error)
	{
		OnFailure("TokenSupplies Failure", Error);
	};
	FGetTokenSuppliesArgs args;
	args.contractAddress = GTestingContractAddress;
	args.includeMetaData = true;
	Indexer->GetTokenSupplies(GTestingChainID, args, GenericSuccess, GenericFailure);
}

void GetTokenSuppliesMapTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTokenSuppliesMapReturn> GenericSuccess = [OnSuccess](const FGetTokenSuppliesMapReturn tokenSuppliesMap)
	{
		OnSuccess("Received TokenSuppliesMap Data");
		if (GPrintAll)
		{
			FString ret = UIndexerSupport::StructToString<FGetTokenSuppliesMapReturn>(tokenSuppliesMap);
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
	Indexer->GetTokenSuppliesMap(GTestingChainID, args, GenericSuccess, GenericFailure);
}

void GetBalanceUpdatesTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FGetBalanceUpdatesReturn> GenericSuccess = [OnSuccess](const FGetBalanceUpdatesReturn balanceUpdates)
	{
		OnSuccess("Received balance Updates Data");
		if (GPrintAll)
		{
			FString ret = UIndexerSupport::StructToString<FGetBalanceUpdatesReturn>(balanceUpdates);
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
	Indexer->GetBalanceUpdates(GTestingChainID, args, GenericSuccess, GenericFailure);
}

void GetTransactionHistoryTest(UIndexer* Indexer, TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure)
{
	const TSuccessCallback<FGetTransactionHistoryReturn> GenericSuccess = [OnSuccess](const FGetTransactionHistoryReturn transactionHistory)
	{
		OnSuccess("Received transactionHistory Data");
		if (GPrintAll)
		{
			FString ret = UIndexerSupport::StructToString<FGetTransactionHistoryReturn>(transactionHistory);
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
	Indexer->GetTransactionHistory(GTestingChainID, args, GenericSuccess, GenericFailure);
}

void EmptyResponseTest(UIndexer * Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("EmptyResponseTest"));
	FString args = "{}";
	FPingReturn rep = Indexer->BuildResponse<FPingReturn>(args);
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

void InvalidResponseTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Invalid Response Test"));
	FString args = "+)0asfjlkj;ksjfa{sdfkaljsa;f[}}}skaflasjfd;";
	FPingReturn rep = Indexer->BuildResponse<FPingReturn>(args);
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

void WrongResponseReceivedTest1(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Wrong Response Received Test 1"));
	FString args = "{\"wrong_arg\" : true}";
	FPingReturn rep = Indexer->BuildResponse<FPingReturn>(args);
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

void WrongResponseReceivedTest2(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Wrong Response Received Test 2"));
	FString args = "{ [ \"alt1\" , \"alt2\" , \"alt3\" ] }";
	FPingReturn rep = Indexer->BuildResponse<FPingReturn>(args);
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
void PingParsingTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Ping Parsing Test"));
	FString args = "{\"status\":true}";
	FPingReturn rep = Indexer->BuildResponse<FPingReturn>(args);
	args = UIndexerSupport::SimplifyString(args);

	//convert response to string testable string format
	FString repString = UIndexerSupport::StructToSimpleString<FPingReturn>(rep);

	if (GPrintAll)
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

void VersionParsingTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Version Parsing Test"));
	FString args = "{\"version\":{\"webrpcVersion\":\"1.0\" , \"schemaVersion\":\"s1.0\" , \"schemaHash\":\"0x1\" , \"appVersion\":\"a1.0\"}}";
	FVersionReturn rep = Indexer->BuildResponse<FVersionReturn>(args);
	args = UIndexerSupport::SimplifyString(args);

	//convert response to string testable string format
	FString repString = UIndexerSupport::StructToSimpleString<FVersionReturn>(rep);

	if (GPrintAll)
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

void RunTimeStatusParsingTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Runtime Status Parsing Test"));
	FString args = "{\"status\":{\"healthOK\":true,\"indexerEnabled\":true,\"startTime\":\"now\",\"uptime\":10.1,\"ver\":\"1.1\",\"branch\":\"main\",\"commitHash\":\"0x12\",\"chainID\":1,\"checks\":{\"running\":true,\"syncMode\":\"Synced\",\"lastBlockNum\":10}}}";
	FRuntimeStatusReturn rep = Indexer->BuildResponse<FRuntimeStatusReturn>(args);
	args = UIndexerSupport::SimplifyString(args);

	//convert response to string testable string format
	FString repString = UIndexerSupport::StructToSimpleString<FRuntimeStatusReturn>(rep);

	if (GPrintAll)
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

void GetChainIDParsingTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Chain ID Parsing Test"));
	FString args = "{\"chainID\":169}";
	FGetChainIDReturn rep = Indexer->BuildResponse<FGetChainIDReturn>(args);
	args = UIndexerSupport::SimplifyString(args);

	//convert response to string testable string format
	FString repString = UIndexerSupport::StructToSimpleString<FGetChainIDReturn>(rep);

	if (GPrintAll)
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

void GetEtherBalanceParsingTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Ether Balance Parsing Test"));
	FString args = "{\"balance\":{\"accountAddress\":\"main_address\",\"balanceWei\":101}}";
	FGetEtherBalanceReturn rep = Indexer->BuildResponse<FGetEtherBalanceReturn>(args);
	args = UIndexerSupport::SimplifyString(args);

	//convert response to string testable string format
	FString repString = UIndexerSupport::StructToSimpleString<FGetEtherBalanceReturn>(rep);

	if (GPrintAll)
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

void GetTokenBalanceParsingTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Token Balance Parsing Test"));
	FString args = "{\"page\":{\"page\":10,\"column\":\"left\",\"before\":\"b1\",\"after\":\"a1\",\"sort\":[{\"column\":\"left\",\"order\":\"DESC\"}],\"pageSize\":64,\"more\":true},\"balances\":[{\"id\":15,\"contractAddress\":\"0xc1\",\"contractType\":\"ERC20\",\"accountAddress\":\"0xa1\",\"tokenID\":11,\"balance\":16384,\"blockHash\":\"0xb1\",\"blockNumber\":1,\"updateID\":3,\"chainId\":137,\"contractInfo\":{\"chainId\":137,\"address\":\"0xa11\",\"name\":\"coin\",\"type\":\"t1\",\"symbol\":\"%\",\"decimals\":101,\"logoURI\":\"http://stuff.ca\",\"extensions\":{\"link\":\"https://that.com\",\"description\":\"extension\",\"ogImage\":\"uint8[]\",\"originChainId\":137,\"originAddress\":\"http://origin.ca\",\"blacklist\":true}},\"tokenMetaData\":{\"tokenId\":101,\"contractAddress\":\"0xc112\",\"name\":\"testing_name\",\"description\":\"some_desc_stuff\",\"image\":\"string_image_data\",\"decimals\":101,\"video\":\"video data\",\"audio\":\"audo_data\",\"image_data\":\"image_data\",\"external_url\":\"external_url_data\",\"background_color\":\"red\",\"animation_url\":\"http://anim.ca\",\"properties\":{\"p1\":10,\"p2\":\"prop_2\",\"p3\":1},\"attributes\":[{\"a11\":\"a\",\"a12\":\"b\"},{\"a21\":\"c\",\"a22\":\"d\",\"a23\":\"e\"}]}}]}";
	FGetTokenBalancesReturn rep = Indexer->BuildResponse<FGetTokenBalancesReturn>(args);
	args = UIndexerSupport::SimplifyString(args);

	//convert response to string testable string format
	//because this objects uses custom setup and parsing we must use custom getting to test with it! unreal won't be able 
	//to parse it completely
	FString repString = UIndexerSupport::JsonToSimpleString(rep.GetJson());

	if (GPrintAll)
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

void GetTokenSuppliesParsingTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("get Token Supplies Parsing Test"));
	FString args = "{\"page\":{\"page\":10,\"column\":\"left\",\"before\":\"b1\",\"after\":\"a1\",\"sort\":[{\"column\":\"left\",\"order\":\"DESC\"}],\"pageSize\":64,\"more\":true},\"contractType\":\"ERC1155\",\"tokenIDs\":[{\"tokenID\":69,\"supply\":\"supply_data\",\"chainId\":9,\"contractInfo\":{\"chainId\":137,\"address\":\"0xa11\",\"name\":\"coin\",\"type\":\"t1\",\"symbol\":\"%\",\"decimals\":101,\"logoURI\":\"http://stuff.ca\",\"extensions\":{\"link\":\"https://that.com\",\"description\":\"extension\",\"ogImage\":\"uint8[]\",\"originChainId\":137,\"originAddress\":\"http://origin.ca\",\"blacklist\":true}},\"tokenMetaData\":{\"tokenId\":101,\"contractAddress\":\"0xc112\",\"name\":\"testing_name\",\"description\":\"some_desc_stuff\",\"image\":\"string_image_data\",\"decimals\":101,\"video\":\"video data\",\"audio\":\"audo_data\",\"image_data\":\"image_data\",\"external_url\":\"external_url_data\",\"background_color\":\"red\",\"animation_url\":\"http://anim.ca\",\"properties\":{\"p1\":10,\"p2\":\"prop_2\",\"p3\":1},\"attributes\":[{\"a11\":\"a\",\"a12\":\"b\"},{\"a21\":\"c\",\"a22\":\"d\",\"a23\":\"e\"}]}}]}";
	FGetTokenSuppliesReturn rep = Indexer->BuildResponse<FGetTokenSuppliesReturn>(args);
	args = UIndexerSupport::SimplifyString(args);

	//convert response to string testable string format
	//because this objects uses custom setup and parsing we must use custom getting to test with it! unreal won't be able 
	//to parse it completely
	FString repString = UIndexerSupport::JsonToSimpleString(rep.GetJson());

	if (GPrintAll)
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

void GetTokenSuppliesMapParsingTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("get Token Supplies Map Parsing Test"));
	FString args = "{\"supplies\":{\"key1\":[{\"tokenID\":101,\"supply\":\"supply_data\",\"chainId\":137,\"contractInfo\":{\"chainId\":137,\"address\":\"0xa11\",\"name\":\"coin\",\"type\":\"t1\",\"symbol\":\"%\",\"decimals\":101,\"logoURI\":\"http://stuff.ca\",\"extensions\":{\"link\":\"https://that.com\",\"description\":\"extension\",\"ogImage\":\"uint8[]\",\"originChainId\":137,\"originAddress\":\"http://origin.ca\",\"blacklist\":true}},\"tokenMetaData\":{\"tokenId\":101,\"contractAddress\":\"0xc112\",\"name\":\"testing_name\",\"description\":\"some_desc_stuff\",\"image\":\"string_image_data\",\"decimals\":101,\"video\":\"video data\",\"audio\":\"audo_data\",\"image_data\":\"image_data\",\"external_url\":\"external_url_data\",\"background_color\":\"red\",\"animation_url\":\"http://anim.ca\",\"properties\":{\"p1\":10,\"p2\":\"prop_2\",\"p3\":1},\"attributes\":[{\"a11\":\"a\",\"a12\":\"b\"},{\"a21\":\"c\",\"a22\":\"d\",\"a23\":\"e\"}]}}]}}";
	FGetTokenSuppliesMapReturn rep = Indexer->BuildResponse<FGetTokenSuppliesMapReturn>(args);
	args = UIndexerSupport::SimplifyString(args);

	//convert response to string testable string format
	//because this objects uses custom setup and parsing we must use custom getting to test with it! unreal won't be able 
	//to parse it completely
	FString repString = UIndexerSupport::JsonToSimpleString(rep.GetJson());

	if (GPrintAll)
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

void GetBalanceUpdatesParsingTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("get Balance Updates Parsing Test"));
	FString args = "{\"page\":"+GTestingPage+",\"balances\":["+GTestingBalanceData+"]}";
	FGetBalanceUpdatesReturn rep = Indexer->BuildResponse<FGetBalanceUpdatesReturn>(args);
	args = UIndexerSupport::SimplifyString(args);

	//convert response to string testable string format
	//because this objects uses custom setup and parsing we must use custom getting to test with it! unreal won't be able 
	//to parse it completely
	FString repString = UIndexerSupport::JsonToSimpleString(rep.GetJson());

	if (GPrintAll)
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

void GetTransactionHistoryParsingTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("get Transaction History Parsing Test"));
	FString args = "{\"page\":"+GTestingPage+",\"transactions\":["+GTestingTransactionData+"]}";
	FGetTransactionHistoryReturn rep = Indexer->BuildResponse<FGetTransactionHistoryReturn>(args);
	args = UIndexerSupport::SimplifyString(args);

	//convert response to string testable string format
	//because this objects uses custom setup and parsing we must use custom getting to test with it! unreal won't be able 
	//to parse it completely
	FString repString = UIndexerSupport::JsonToSimpleString(rep.GetJson());

	if (GPrintAll)
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
void TokenBalanceMaxArgsTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Token Balance [MAX] Args Parsing Test"));
	FString testArgs = "{\"accountAddress\":\""+GTestingAddress+"\",\"contractAddress\":\""+GTestingContractAddress+"\",\"includeMetaData\":true,\"page\":"+GTestingPage+"}";
	
	FGetTokenBalancesArgs args;
	args.accountAddress = GTestingAddress;
	args.contractAddress = GTestingContractAddress;
	args.includeMetaData = true;
	args.page = BuildTestPage();
	
	FString stringArgs = UIndexerSupport::SimplifyStringParsable(Indexer->BuildArgs<FGetTokenBalancesArgs>(args));
	testArgs = UIndexerSupport::SimplifyStringParsable(testArgs);

	if (GPrintAll)
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

void TokenBalanceMinArgsTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Token Balance [MIN] Args Parsing Test"));
	FString testArgs = "{\"accountAddress\":\"" + GTestingAddress + "\",\"includeMetaData\":false}";

	FGetTokenBalancesArgs args;
	args.accountAddress = GTestingAddress;

	FString stringArgs = UIndexerSupport::SimplifyStringParsable(Indexer->BuildArgs<FGetTokenBalancesArgs>(args));
	testArgs = UIndexerSupport::SimplifyStringParsable(testArgs);

	if (GPrintAll)
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
FPage BuildTestPage()
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

TMap<FString, FTokenList> BuildTokenMap()
{
	TMap<FString, FTokenList> ret;
	//{\"keyOne\":[\"value1\",\"value2\"]}
	FTokenList rItem;//array of strings!
	rItem.token_list.Add("value1");
	rItem.token_list.Add("value2");
	ret.Add(TPair<FString,FTokenList>("keyOne",rItem));
	return ret;
}

FTransactionHistoryFilter BuildHistoryFilter()
{
	FTransactionHistoryFilter ret;
	ret.accountAddress = GTestingAddress;
	ret.contractAddress = GTestingContractAddress;
	ret.accountAddresses.Add(GTestingAddress);
	ret.contractAddresses.Add(GTestingContractAddress);
	ret.transactionHashes.Add("0x123abc");
	ret.metaTransactionIDs.Add("0xTEST");
	ret.fromBlock = 10;
	ret.toBlock = 101;
	return ret;
}

void TokenSuppliesArgsTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Token Supplies Args Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"contractAddress\":\""+GTestingContractAddress+"\",\"includeMetaData\":true,\"page\":"+GTestingPage+"}";

	FGetTokenSuppliesArgs args;
	args.contractAddress = GTestingContractAddress;
	args.includeMetaData = true;
	args.page = BuildTestPage();

	FString stringArgs = UIndexerSupport::SimplifyStringParsable(Indexer->BuildArgs<FGetTokenSuppliesArgs>(args));
	testArgs = UIndexerSupport::SimplifyStringParsable(testArgs);

	if (GPrintAll)
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

void TokenSuppliesMapArgsTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("Token Supplies Map Args Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"tokenMap\":"+GTestTokenMap+",\"includeMetaData\":true}";
	FGetTokenSuppliesMapArgs args;
	args.tokenMap = BuildTokenMap();
	args.includeMetaData = true;
	FString stringArgs = UIndexerSupport::SimplifyStringParsable(Indexer->BuildArgs<FGetTokenSuppliesMapArgs>(args));
	testArgs = UIndexerSupport::SimplifyStringParsable(testArgs);

	if (GPrintAll)
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

void BalanceUpdatesMaxArgsTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("balance Updates [MAX] Args Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"contractAddress\":\""+GTestingContractAddress+"\",\"lastUpdateID\":10,\"page\":"+GTestingPage+"}";
	FGetBalanceUpdatesArgs args;
	args.contractAddress = GTestingContractAddress;
	args.lastUpdateID = 10;
	args.page = BuildTestPage();

	FString stringArgs = UIndexerSupport::SimplifyStringParsable(Indexer->BuildArgs<FGetBalanceUpdatesArgs>(args));
	testArgs = UIndexerSupport::SimplifyStringParsable(testArgs);

	if (GPrintAll)
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

void BalanceUpdatesMinArgsTest(UIndexer * Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("balance Updates [MIN] Args Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"contractAddress\":\"" + GTestingContractAddress + "\"}";
	FGetBalanceUpdatesArgs args;
	args.contractAddress = GTestingContractAddress;

	FString stringArgs = UIndexerSupport::SimplifyStringParsable(Indexer->BuildArgs<FGetBalanceUpdatesArgs>(args));
	testArgs = UIndexerSupport::SimplifyStringParsable(testArgs);

	if (GPrintAll)
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

void TransactionHistoryArgsMaxTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("transaction History Args [Max] Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"filter\":"+ GTestHistoryFilterMax +",\"page\":"+GTestingPage+",\"includeMetaData\":true}";
	FGetTransactionHistoryArgs args;
	args.filter = BuildHistoryFilter();
	args.page = BuildTestPage();
	args.includeMetaData = true;

	FString stringArgs = UIndexerSupport::SimplifyStringParsable(Indexer->BuildArgs<FGetTransactionHistoryArgs>(args));
	testArgs = UIndexerSupport::SimplifyStringParsable(testArgs);

	if (GPrintAll)
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

void TransactionHistoryArgsMinTest(UIndexer* Indexer)
{
	UE_LOG(LogTemp, Display, TEXT("==========================================================="));
	UE_LOG(LogTemp, Display, TEXT("transaction History Args [Min] Parsing Test"));
	//this will mirror args!
	FString testArgs = "{\"filter\":" + GTestHistoryFilterMin + ",\"includeMetaData\":true}";
	FGetTransactionHistoryArgs args;
	args.filter.accountAddress = GTestingAddress;
	args.includeMetaData = true;

	FString stringArgs = UIndexerSupport::SimplifyStringParsable(Indexer->BuildArgs<FGetTransactionHistoryArgs>(args));
	testArgs = UIndexerSupport::SimplifyStringParsable(testArgs);

	if (GPrintAll)
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
	EmptyResponseTest(indexer);
	InvalidResponseTest(indexer);
	WrongResponseReceivedTest1(indexer);
	WrongResponseReceivedTest2(indexer);
	UE_LOG(LogTemp, Display, TEXT("End of Failure Tests"));
	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));

	//parsing tests//
	//buildResponse parsing tests//
	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));
	UE_LOG(LogTemp, Display, TEXT("Start of Response Tests"));
	PingParsingTest(indexer);
	VersionParsingTest(indexer);
	RunTimeStatusParsingTest(indexer);
	GetChainIDParsingTest(indexer);
	GetEtherBalanceParsingTest(indexer);
	GetTokenBalanceParsingTest(indexer);
	GetTokenSuppliesParsingTest(indexer);
	GetTokenSuppliesMapParsingTest(indexer);
	GetBalanceUpdatesParsingTest(indexer);
	GetTransactionHistoryParsingTest(indexer);
	//buildResponse parsing tests//
	UE_LOG(LogTemp, Display, TEXT("End of Response Tests"));
	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));

	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));
	UE_LOG(LogTemp, Display, TEXT("Start of Argument Tests"));
	//buildArgs parsing tests//
	TokenBalanceMaxArgsTest(indexer);
	TokenBalanceMinArgsTest(indexer);
	TokenSuppliesArgsTest(indexer);
	TokenSuppliesMapArgsTest(indexer);
	BalanceUpdatesMaxArgsTest(indexer);
	BalanceUpdatesMinArgsTest(indexer);
	TransactionHistoryArgsMaxTest(indexer);
	TransactionHistoryArgsMinTest(indexer);
	//buildArgs parsing tests//
	UE_LOG(LogTemp, Display, TEXT("End of Argument Tests"));
	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));

	UE_LOG(LogTemp, Display, TEXT("||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"));
	UE_LOG(LogTemp, Display, TEXT("Start of System Tests"));
	//system tests//
	PingTest(indexer, OnSuccess, OnFailure);
	VersionTest(indexer, OnSuccess, OnFailure);
	RunTimeStatusTest(indexer, OnSuccess, OnFailure);
	GetChainIDTest(indexer, OnSuccess, OnFailure);
	GetEtherBalanceTest(indexer, OnSuccess, OnFailure);
	GetTokenBalanceTest(indexer, OnSuccess, OnFailure);
	GetTokenSuppliesTest(indexer,OnSuccess, OnFailure);
	GetTokenSuppliesMapTest(indexer, OnSuccess, OnFailure);
	GetBalanceUpdatesTest(indexer, OnSuccess, OnFailure);
	GetTransactionHistoryTest(indexer, OnSuccess, OnFailure);
	return;//done
}