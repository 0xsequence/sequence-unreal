#pragma once

#include "CoreMinimal.h"
#include "Util/Async.h"
#include "Indexer/Structs/Struct_Data.h"
#include "Engine/Texture2D.h"
#include "SequenceIndexer.generated.h"

/**
 * 
 */
UCLASS()

class SEQUENCEPLUGIN_API USequenceIndexer : public UObject
{
	GENERATED_BODY()

private:
	const FString PATH = "/rpc/Indexer/";
	
//private functions

	/*
		Creates the URL from a given chainID and endpoint
	*/
	FString Url(const FString& ChainID,const FString& EndPoint) const;

	/*
		Returns the host name
	*/
	static FString HostName(FString ChainID);

	/*
		Used to send an HTTPPost req to a the sequence app
		@return the content of the post response
	*/
	void HTTPPost(const FString& ChainID,const FString& Endpoint,const FString& Args, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;

	//end of private functions
public:
	//public functions

	/*
	Here we take in text and convert it to a Struct of type T if possible
	@return a Struct of type T
	
	NOTE: Because unreal doesn't support nested TArrays and TMaps I had to use special implementations
	for some data structures inorder for them to parse properly
	*/
	template < typename T > T BuildResponse(FString Text);

	/*
	Here we take in a struct and convert it straight into a json object String
	@Param (T) Struct_in the struct we are converting to a json object string
	@Return the JSON Object String
	*/
	template < typename T> FString BuildArgs(T StructIn);
	
	USequenceIndexer();

	/*
		Used to get a ping back from the Chain
	*/
	void Ping(FString ChainID, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure);

	/*
		Used to get version data back from the Chain
	*/
	void Version(FString ChainID, TSuccessCallback<FSeqVersion> OnSuccess, const FFailureCallback& OnFailure);

	/*
		Used to get the runtime status of the Chain
	*/
	void RuntimeStatus(FString ChainID, TSuccessCallback<FSeqRuntimeStatus> OnSuccess, const FFailureCallback& OnFailure);

	/*
		Used to get the chainID from the Chain
	*/
	void GetChainID(FString ChainID, TSuccessCallback<int64> OnSuccess, const FFailureCallback& OnFailure);

	/*
		Used to get the native token balance from the Chain
		@param 1st the ChainID
		@param 2nd the accountAddr we want to get the balance for
		@return the Balance ASYNC calls
	*/
	void GetNativeTokenBalance(FString ChainID, FString AccountAddr, TSuccessCallback<FSeqEtherBalance> OnSuccess, const FFailureCallback& OnFailure);

	/*
		Gets the token balances from the Chain
	*/
	void GetTokenBalances(FString ChainID, const FSeqGetTokenBalancesArgs& Args, TSuccessCallback<FSeqGetTokenBalancesReturn> OnSuccess, const FFailureCallback& OnFailure);

	/*
		gets the token supplies from the Chain
	*/
	void GetTokenSupplies(FString ChainID, const FSeqGetTokenSuppliesArgs& Args, TSuccessCallback<FSeqGetTokenSuppliesReturn> OnSuccess, const FFailureCallback& OnFailure);

	/*
		gets the token supplies map from the Chain
	*/
	void GetTokenSuppliesMap(FString ChainID, const FSeqGetTokenSuppliesMapArgs& Args, TSuccessCallback<FSeqGetTokenSuppliesMapReturn> OnSuccess, const FFailureCallback& OnFailure);
	
	/*
		get transaction history from the Chain
	*/
	void GetTransactionHistory(FString ChainID, const FSeqGetTransactionHistoryArgs& Args, TSuccessCallback<FSeqGetTransactionHistoryReturn> OnSuccess, const FFailureCallback& OnFailure);
	
	/*
	 *	Converts a TArray<FTokenBalance> Into a TMap<int64, FTokenBalance>
	 *	where the key is the TokenID
	 */
	static TMap<FString, FSeqTokenBalance> GetTokenBalancesAsMap(TArray<FSeqTokenBalance> Balances);
	
//end of public functions
};