// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Struct_Data.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "Http.h"
#include "HttpManager.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Sequence_Backend_Manager.h"
#include "ObjectHandler.h"
#include "Indexer.generated.h"

/**
 * 
 */
UCLASS()

class SEQUENCEPLUGIN_API UIndexer : public UObject
{
	GENERATED_BODY()

private:
	const FString PATH = "/rpc/Indexer/";
	TMap<int64, FString> Indexernames;
	ASequence_Backend_Manager * bck_mngr;

//private functions
	
	/*
		Creates the URL from a given chainID and endpoint
	*/
	FString Url(int64 chainID, FString endpoint);

	/*
		Returns the host name
	*/
	FString HostName(int64 chainID);

	/*
		Used to send an HTTPPost req to a the sequence app
		@return the content of the post response
	*/
	TFuture<FString> HTTPPost(int64 chainID, FString endpoint, FString args);

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

//end of private functions

public:

//public functions

	void setup(ASequence_Backend_Manager* manager_ref);

	/*
		Used for testing out the core components of the indexer
	*/
	TArray<UTexture2D*> testing();

	UTexture2D* get_image_data(FString URL);

	UTexture2D* build_image_data(TArray<uint8> img_data, FString URL);

	EImageFormat get_image_format(FString URL);

	/*
		Used to remove all \n, \r, \t and spaces from a json string for testing!
		@param firstParam (The already made json object in FString form)
		@ret void (this is inline so all effects are carried on the presented obj.
	*/
	void Remove_Json_SNRT_INLINE(FString * json_string_in);

	/*
		Used for testing json parsing
	*/
	template <typename T> bool Test_Json_Parsing(FString json_in, FString type);

	/*
		General purpose constuctor
	*/
	UIndexer();

	/*
		Used to get a ping back from the sequence app
	*/
	TFuture<bool> Ping(int64 chainID);

	/*
		Used to get version data back from the sequence app
	*/
	TFuture<FVersion> Version(int64 chainID);

	/*
		Used to get the runtime status of the sequence app
	*/
	TFuture<FRuntimeStatus> RunTimeStatus(int64 chainID);

	/*
		Used to get the chainID from the sequence app
	*/
	TFuture<int64> GetChainID(int64 chainID);

	/*
		Used to get the Ether balance from the sequence app
		@param 1st the ChainID
		@param 2nd the accountAddr we want to get the balance for
		@return the Balance ASYNC calls (update ether balance in the bck_mngr when done processing)
	*/
	void GetEtherBalance(int64 chainID, FString accountAddr);

	/*
		Gets the token balances from the sequence app
	*/
	TFuture<FGetTokenBalancesReturn> GetTokenBalances(int64 chainID, FGetTokenBalancesArgs args);

	/*
		gets the token supplies from the sequence app
	*/
	TFuture<FGetTokenSuppliesReturn> GetTokenSupplies(int64 chainID, FGetTokenSuppliesArgs args);

	/*
		gets the token supplies map from the sequence app
	*/
	TFuture<FGetTokenSuppliesMapReturn> GetTokenSuppliesMap(int64 chainID, FGetTokenSuppliesMapArgs args);
	
	/*
		Get the balance updates from the sequence app
	*/
	TFuture<FGetBalanceUpdatesReturn> GetBalanceUpdates(int64 chainID, FGetBalanceUpdatesArgs args);

	/*
		get transaction history from the sequence app
	*/
	TFuture<FGetTransactionHistoryReturn> GetTransactionHistory(int64 chainID, FGetTransactionHistoryArgs args);

	/*
	* Used for making Async requests!
	*/
	void async_request(FString url, FString json, void (UIndexer::* handler)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful));

	void async_request_test(FString url, FString json,int32 i_index, void(UIndexer::* handler)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful));

	//async handlers
	private:

		void get_ether_handler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

//end of public functions
};