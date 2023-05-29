// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Struct_Data.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Http.h"
#include "Indexer.generated.h"
/**
 * 
 */
UCLASS()

class SEQUENCEPLUGIN_API UIndexer : public UObject
{
	GENERATED_BODY()

private:
	//need static data here!

	//not sure what this path is supposed to go to?
	const FString PATH = "/rpc/Indexer/";

	TMap<int64, FString> Indexernames;

//private functions
	
	FString Url(int64 chainID, FString endpoint);

	FString HostName(int64 chainID);

	FString HTTPPost(int64 chainID, FString endpoint, FString args);

	template < typename T > T BuildResponse(FString text);

//end of private functions

public:

//public functions

	void testing();

	/*
		Used to remove all \n, \r, \t and spaces from a json string for testing!
		@param firstParam (The already made json object in FString form)
		@ret void (this is inline so all effects are carried on the presented obj.
	*/
	void Remove_Json_SNRT_INLINE(FString * json_string_in);

	template <typename T> bool Test_Json_Parsing(FString json_in, FString type);

	UIndexer();
	UIndexer(FString args);

	bool Ping(int64 chainID);

	FVersion Version(int64 chainID);

	//need to assess task structure
	FRuntimeStatus RunTimeStatus(int64 chainID);

	int64 GetChainID(int64 chainID);//?

	FEtherBalance GetEtherBalance(int64 chainID, FString accountAddr);

	FGetTokenBalancesReturn GetTokenBalances(int64 chainID, FString args);

	FGetTokenSuppliesReturn GetTokenSupplies(int64 chainID, FString args);

	FGetTokenSuppliesMapReturn GetTokenSuppliesMap(int64 chainID, FString args);

	FGetBalanceUpdatesReturn GetBalanceUpdates(int64 chainID, FString args);

	FGetTransactionHistoryReturn GetTransactionHistory(int64 chainID, FString args);

//end of public functions


};

//templated functions
/*
template<class T>
inline T UIndexer::BuildResponse(FString text)
{

	return T();
}
*/