// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "Util/Async.h"
#include "Indexer/Structs/Struct_Data.h"
#include "Dom/JsonObject.h"
#include "Engine/Texture2D.h"
#include "Indexer.generated.h"

/**
 * 
 */
UCLASS()

class SEQUENCEPLUGIN_API UIndexer : public UObject
{
	GENERATED_BODY()

public:
	static TMap<int64, FString> GetIndexerNames();
	static FString GetIndexerName(int64 chainId);
private:
	static const TMap<int64, FString> IndexerNames;
	const FString PATH = "/rpc/Indexer/";
	
//private functions

	/*
		Creates the URL from a given chainID and endpoint
	*/
	FString Url(const int64& ChainID,const FString& Endpoint);

	/*
		Returns the host name
	*/
	FString HostName(int64 ChainID);

	/*
		Used to send an HTTPPost req to a the sequence app
		@return the content of the post response
	*/
	void HTTPPost(const int64& ChainID,const FString& Endpoint,const FString& Args, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	//end of private functions
public:
	//public functions

	/*
	Here we take in text and convert it to a Struct of type T if possible
	@return a Struct of type T
	
	NOTE: Because unreal doesn't support nested Tarrays and Tmaps I had to use special implementations
	for some data structures inorder for them to parse properly
	*/
	template < typename T > T BuildResponse(FString Text);

	/*
	Here we take in a struct and convert it straight into a json object String
	@Param (T) Struct_in the struct we are converting to a json object string
	@Return the JSON Object String
	*/
	template < typename T> FString BuildArgs(T StructIn);
	
	UIndexer();

	/*
		Used to get a ping back from the Chain
	*/
	void Ping(int64 ChainID, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get version data back from the Chain
	*/
	void Version(int64 ChainID, TSuccessCallback<FVersion> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the runtime status of the Chain
	*/
	void RunTimeStatus(int64 ChainID, TSuccessCallback<FRuntimeStatus> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the chainID from the Chain
	*/
	void GetChainID(int64 ChainID, TSuccessCallback<int64> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the Ether balance from the Chain
		@param 1st the ChainID
		@param 2nd the accountAddr we want to get the balance for
		@return the Balance ASYNC calls (update ether balance in the bck_mngr when done processing)
	*/
	void GetEtherBalance(int64 ChainID, FString AccountAddr, TSuccessCallback<FEtherBalance> OnSuccess, FFailureCallback OnFailure);

	/*
		Gets the token balances from the Chain
	*/
	void GetTokenBalances(int64 ChainID, FGetTokenBalancesArgs Args, TSuccessCallback<FGetTokenBalancesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		gets the token supplies from the Chain
	*/
	void GetTokenSupplies(int64 ChainID, FGetTokenSuppliesArgs Args, TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		gets the token supplies map from the Chain
	*/
	void GetTokenSuppliesMap(int64 ChainID, FGetTokenSuppliesMapArgs Args, TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess, FFailureCallback OnFailure);
	
	/*
		Get the balance updates from the Chain
	*/
	void GetBalanceUpdates(int64 ChainID, FGetBalanceUpdatesArgs Args, TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		get transaction history from the Chain
	*/
	void GetTransactionHistory(int64 ChainID, FGetTransactionHistoryArgs Args, TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess, FFailureCallback OnFailure);

//end of public functions
};