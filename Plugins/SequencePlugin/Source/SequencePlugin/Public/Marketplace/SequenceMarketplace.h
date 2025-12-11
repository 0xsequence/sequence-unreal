#pragma once 

#include "CoreMinimal.h"
#include "Util/Async.h"
#include "Marketplace/Structs/Struct_Data.h"
#include "Structs/SeqGetCollectibleOrderArgs.h"
#include "Structs/SeqListCollectibleListingsReturn.h"
#include "Structs/SeqListCollectibleOffersReturn.h"
#include "Structs/SeqListCurrenciesReturn.h"
#include "SequenceMarketplace.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API USequenceMarketplace : public UObject
{
	GENERATED_BODY()

private:

	const FString PATH = "/rpc/Marketplace/";

	TArray<FSeqCollectibleOrder> OrderArray;
	TArray<FSeqOrder> RawOrderArray;

	//private functions

		/*
			Creates the URL from a given chainID and endpoint
		*/
	FString Url(const FString& ChainID, const FString& EndPoint) const;

	/*
		Returns the host name
	*/
	static FString HostName(FString ChainID);

	// HELPERS
	void ListAllCollectibleListingsWithLowestPriceListingsFirstHelper(const FString ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TFunction<void(TArray<FSeqCollectibleOrder>, bool)> OnSuccess, const FFailureCallback& OnFailure);
	void ListAllCollectibleOffersWithHighestPricedOfferFirstHelper(const FString ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TFunction<void(TArray<FSeqCollectibleOrder>, bool)> OnSuccess, const FFailureCallback& OnFailure);
	void GetCollectibleOrder(FString ChainID, const FString& Endpoint, const FSeqGetCollectibleOrderArgs& Args,
	                         TSuccessCallback<FSeqCollectibleOrder> OnSuccess, const FFailureCallback& OnFailure);
	void ListAllListingsForCollectibleHelper(const FString ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TFunction<void(TArray<FSeqOrder>, bool)> OnSuccess, const FFailureCallback& OnFailure);
	void ListAllOffersForCollectibleHelper(const FString ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TFunction<void(TArray<FSeqOrder>, bool)> OnSuccess, const FFailureCallback& OnFailure);
	
public:
	
	/*
		Used to send an HTTPPost req to a the sequence app
		@return the content of the post response
	*/
	void HTTPPost(const FString& ChainID, const FString& Endpoint, const FString& Args, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const;

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

    USequenceMarketplace();

	/**
	 * Lists the marketplace currencies
	 * @param ChainID the id of the chain
	 * @param OnSuccess handler for success, takes in a FSeqListCurrenciesReturn
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void ListCurrencies(const FString ChainID, TSuccessCallback<FSeqListCurrenciesReturn> OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Lists collectible listings with the lowest price listings first
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract
	 * @param Filter the filter to apply
	 * @param Page the page to apply
	 * @param OnSuccess handler for success, takes in a FSeqListCollectiblesReturn
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void GetCollectiblesWithLowestListingsFirst(const FString ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TSuccessCallback<FSeqListCollectiblesReturn> OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Lists all collectible listings with the lowest price listings first
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract
	 * @param Filter the filter to apply
	 * @param OnSuccess handler for success, takes in a TArray of FSeqCollectibleOrder
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void GetAllCollectiblesWithLowestListingsFirst(const FString ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * List collectible offers with the highest priced offer first
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract
	 * @param Filter the filter to apply
	 * @param Page the page to apply
	 * @param OnSuccess handler for success, takes in a FSeqListCollectiblesReturn
	 * @param OnFailure	handler for failure, takes in a FSequenceError
	 */
	void GetCollectiblesWithHighestPricedOffersFirst(const FString ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TSuccessCallback<FSeqListCollectiblesReturn> OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Lists all collectible offers with the highest priced offer first
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract
	 * @param Filter the filter to apply
	 * @param OnSuccess handler for success, takes in a TArray of FSeqCollectibleOrder
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void ListAllCollectibleOffersWithHighestPricedOfferFirst(const FString ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Gets the lowest price offer for a collectible
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract
	 * @param TokenID the id of the token
	 * @param Filter the filter to apply
	 * @param OnSuccess handler for success, takes in a FSeqCollectibleOrder
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void GetLowestPriceOfferForCollectible(const FString ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Gets the highest price offer for a collectible
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract
	 * @param TokenID the id of the token
	 * @param Filter the filter to apply
	 * @param OnSuccess handler for success, takes in a FSeqCollectibleOrder
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void GetHighestPriceOfferForCollectible(const FString ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Gets the lowest price listing for a collectible
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract
	 * @param TokenID the id of the token
	 * @param Filter the filter to apply
	 * @param OnSuccess handler for success, takes in a FSeqCollectibleOrder
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void GetLowestPriceListingForCollectible(const FString ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Get the highest price listing for a collectible
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract
	 * @param TokenID the id of the token
	 * @param Filter the filter to apply
	 * @param OnSuccess handler for success, takes in a FSeqCollectibleOrder
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void GetHighestPriceListingForCollectible(const FString ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Get listings for a collectible
	 * @param ChainID the id of the chain 
	 * @param ContractAddress the address of the contract
	 * @param TokenID the id of the token
	 * @param Filter the filter to apply
	 * @param Page the page to apply
	 * @param OnSuccess handler for success, takes in a FSeqListCollectibleListingsReturn
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void ListListingsForCollectible(const FString ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TSuccessCallback<FSeqListCollectibleListingsReturn> OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Get all listings for a collectible
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract
	 * @param TokenID the id of the token
	 * @param Filter the filter to apply
	 * @param OnSuccess handler for success, takes in a TArray of FSeqCollectibleOrder
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void ListAllListingsForCollectible(const FString ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, TSuccessCallback<TArray<FSeqOrder>> OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Get offers for a collectible
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract 
	 * @param TokenID the id of the token
	 * @param Filter the filter to apply
	 * @param Page the page to apply
	 * @param OnSuccess handler for success, takes in a FSeqListCollectibleOffersReturn
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void ListOffersForCollectible(const FString ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TSuccessCallback<FSeqListCollectibleOffersReturn> OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Get all offers for a collectible
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract
	 * @param TokenID the id of the token
	 * @param Filter the filter to apply
	 * @param OnSuccess handler for success, takes in a TArray of FSeqCollectibleOrder
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void ListAllOffersForCollectible(const FString ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, TSuccessCallback<TArray<FSeqOrder>> OnSuccess, const FFailureCallback& OnFailure);

	/**
	 * Get the floor order for a collectible
	 * @param ChainID the id of the chain
	 * @param ContractAddress the address of the contract
	 * @param Filter the filter to apply
	 * @param OnSuccess handler for success, takes in a FSeqCollectibleOrder
	 * @param OnFailure handler for failure, takes in a FSequenceError
	 */
	void GetFloorOrder(const FString ChainID, const FString& ContractAddress, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess, const FFailureCallback& OnFailure);
};

