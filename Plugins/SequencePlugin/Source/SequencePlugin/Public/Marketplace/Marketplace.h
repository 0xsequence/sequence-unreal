// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once 

#include "CoreMinimal.h"
#include "Util/Async.h"
#include "Marketplace/Structs/Struct_Data.h"
#include "Structs/SeqGetCollectibleOrderArgs.h"
#include "Structs/SeqListCollectibleListingsReturn.h"
#include "Structs/SeqListCollectibleOffersReturn.h"
#include "Structs/SeqListCurrenciesReturn.h"
#include "Marketplace.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API UMarketplace : public UObject
{
	GENERATED_BODY()

private:

	const FString PATH = "/rpc/Marketplace/";

	TArray<FSeqCollectibleOrder> OrderArray;

	//private functions

		/*
			Creates the URL from a given chainID and endpoint
		*/
	FString Url(const int64& ChainID, const FString& EndPoint) const;

	/*
		Returns the host name
	*/
	static FString HostName(int64 ChainID);

	// HELPERS
	void ListAllCollectibleListingsWithLowestPriceListingsFirstHelper(const int64 ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TFunction<void(TArray<FSeqCollectibleOrder>, bool)> OnSuccess, const FFailureCallback& OnFailure);
	void ListAllCollectibleOffersWithHighestPricedOfferFirstHelper(const int64 ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TFunction<void(TArray<FSeqCollectibleOrder>, bool)> OnSuccess, const FFailureCallback& OnFailure);
	void GetCollectibleOrder(int64 ChainID, const FString& Endpoint, const FSeqGetCollectibleOrderArgs& Args,
	                         TSuccessCallback<FSeqCollectibleOrder> OnSuccess, const FFailureCallback& OnFailure);
	void ListAllListingsForCollectibleHelper(const int64 ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TFunction<void(TArray<FSeqCollectibleOrder>, bool)> OnSuccess, const FFailureCallback& OnFailure);
	void ListAllOffersForCollectibleHelper(const int64 ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TFunction<void(TArray<FSeqCollectibleOrder>, bool)> OnSuccess, const FFailureCallback& OnFailure);
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

	// Nothing
	void ListCurrencies(const int64 ChainID, TSuccessCallback<FSeqListCurrenciesReturn> OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, filter, page 
    void ListCollectibleListingsWithLowestPriceListingsFirst(const int64 ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TSuccessCallback<FSeqListCollectiblesReturn> OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, filter
	void ListAllCollectibleListingsWithLowestPriceListingsFirst(const int64 ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, filter, page
	void ListCollectibleOffersWithHighestPricedOfferFirst(const int64 ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TSuccessCallback<FSeqListCollectiblesReturn> OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, filter
	void ListAllCollectibleOffersWithHighestPricedOfferFirst(const int64 ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, tokenId, filter
	void GetLowestPriceOfferForCollectible(const int64 ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, tokenId, filter
	void GetHighestPriceOfferForCollectible(const int64 ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, tokenId, filter
	void GetLowestPriceListingForCollectible(const int64 ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, tokenId, filter
	void GetHighestPriceListingForCollectible(const int64 ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, tokenId, filter, page
	void ListListingsForCollectible(const int64 ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TSuccessCallback<FSeqListCollectibleListingsReturn> OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, tokenId, filter
	void ListAllListingsForCollectible(const int64 ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, tokenId, filter, page
	void ListOffersForCollectible(const int64 ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TSuccessCallback<FSeqListCollectibleOffersReturn> OnSuccess, const FFailureCallback& OnFailure);
	// Takes address, tokenId, filter
	void ListAllOffersForCollectible(const int64 ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess, const FFailureCallback& OnFailure);
	// takes address, filter
	void GetFloorOrder(const int64 ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess, const FFailureCallback& OnFailure);
};

