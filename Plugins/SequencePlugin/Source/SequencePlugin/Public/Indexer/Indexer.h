// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Util/Async.h"
#include "UObject/NoExportTypes.h"
#include "Indexer/Structs/Struct_Data.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Http.h"
#include "HttpManager.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "SequenceBackendManager.h"
#include "ObjectHandler.h"
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
	FString Url(const int64& chainID,const FString& endpoint);

	/*
		Returns the host name
	*/
	FString HostName(int64 chainID);

	/*
		Used to send an HTTPPost req to a the sequence app
		@return the content of the post response
	*/
	void HTTPPost(const int64& chainID,const FString& endpoint,const FString& args, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure);

	//end of private functions
public:
	//public functions

	/*
	Here we take in text and convert it to a Struct of type T if possible
	@return a Struct of type T
	
	NOTE: Because unreal doesn't support nested Tarrays and Tmaps I had to use special implementations
	for some data structures inorder for them to parse properly
	*/
	template < typename T > T BuildResponse(FString text);

	/*
	Here we take in a struct and convert it straight into a json object String
	@Param (T) Struct_in the struct we are converting to a json object string
	@Return the JSON Object String
	*/
	template < typename T> FString BuildArgs(T struct_in);

	/*
		General purpose constuctor
	*/
	UIndexer();

	/*
		Used to get a ping back from the sequence app
	*/
	void Ping(int64 chainID, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get version data back from the sequence app
	*/
	void Version(int64 chainID, TSuccessCallback<FVersion> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the runtime status of the sequence app
	*/
	void RunTimeStatus(int64 chainID, TSuccessCallback<FRuntimeStatus> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the chainID from the sequence app
	*/
	void GetChainID(int64 chainID, TSuccessCallback<int64> OnSuccess, FFailureCallback OnFailure);

	/*
		Used to get the Ether balance from the sequence app
		@param 1st the ChainID
		@param 2nd the accountAddr we want to get the balance for
		@return the Balance ASYNC calls (update ether balance in the bck_mngr when done processing)
	*/
	void GetEtherBalance(int64 chainID, FString accountAddr, TSuccessCallback<FEtherBalance> OnSuccess, FFailureCallback OnFailure);

	/*
		Gets the token balances from the sequence app
	*/
	void GetTokenBalances(int64 chainID, FGetTokenBalancesArgs args, TSuccessCallback<FGetTokenBalancesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		gets the token supplies from the sequence app
	*/
	void GetTokenSupplies(int64 chainID, FGetTokenSuppliesArgs args, TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		gets the token supplies map from the sequence app
	*/
	void GetTokenSuppliesMap(int64 chainID, FGetTokenSuppliesMapArgs args, TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess, FFailureCallback OnFailure);
	
	/*
		Get the balance updates from the sequence app
	*/
	void GetBalanceUpdates(int64 chainID, FGetBalanceUpdatesArgs args, TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess, FFailureCallback OnFailure);

	/*
		get transaction history from the sequence app
	*/
	void GetTransactionHistory(int64 chainID, FGetTransactionHistoryArgs args, TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess, FFailureCallback OnFailure);

//end of public functions
};