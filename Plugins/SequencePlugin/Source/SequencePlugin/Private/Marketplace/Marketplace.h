// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once 

#include "CoreMinimal.h"
#include "Util/Async.h"
#include "Marketplace/Structs/Struct_Data.h"
#include "Marketplace.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UMarketplace : public UObject
{
	GENERATED_BODY()

private:

	const FString PATH = "/rpc/Marketplace/";

	//private functions

		/*
			Creates the URL from a given chainID and endpoint
		*/
	FString Url(const int64& ChainID, const FString& EndPoint) const;

	/*
		Returns the host name
	*/
	static FString HostName(int64 ChainID);

public:
	/*
		Used to send an HTTPPost req to a the sequence app
		@return the content of the post response
	*/
	void HTTPPost(const int64& ChainID, const FString& Endpoint, const FString& Args, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;


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

    UMarketplace();

    void GetCollectiblesWithLowestListings(const int64 ChainID, const FSeqGetCollectiblesWithLowestListingsArgs& Args, TSuccessCallback<FSeqGetCollectiblesWithLowestListingsReturn> OnSuccess, const FFailureCallback& OnFailure);



};

