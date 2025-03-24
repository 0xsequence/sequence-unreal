// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Checkout/Structs/AdditionalFee.h"
#include "Marketplace/Structs/SeqCollectibleOrder.h"
#include "Structs/SardineEnabledToken.h"
#include "Structs/SardineFiatCurrency.h"
#include "Structs/SardineGetNFTCheckoutTokenArgs.h"
#include "Structs/SardineNFTCheckout.h"
#include "Structs/SardineOrder.h"
#include "Structs/SardinePaymentType.h"
#include "Structs/SardineQuote.h"
#include "Structs/SardineQuoteType.h"
#include "Structs/SardineRegion.h"
#include "Structs/SardineSupportedToken.h"
#include "Structs/SardineToken.h"
#include "Types/ERC1155SaleContract.h"
#include "Types/ERC721SaleContract.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "SardineCheckout.generated.h"

/**
 * 
 */
UCLASS()
class SEQUENCEPLUGIN_API USardineCheckout : public UObject
{
	GENERATED_BODY()

	const FString _sardineCheckoutUrl =
			"https://sardine-checkout.sequence.info?api_url=https://sardine-api.sequence.info&client_token=";

	const FString _sardineSandboxCheckoutUrl =
		"https://sardine-checkout-sandbox.sequence.info?api_url=https://sardine-api-sandbox.sequence.info&client_token=";

	const FString _sardineCheckoutUrlSuffix = "&show_features=true";

	const FString _devUrl = "https://dev-api.sequence.app/rpc/API";
	const FString _prodUrl = "https://api.sequence.app/rpc/API";
	FString _baseUrl = _prodUrl;


	FString Url(const FString& EndPoint) const;

	void SardineGetNFTCHeckoutToken(FSardineGetNFTCheckoutTokenArgs Args, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure);
	
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
	void HTTPPost(const FString& Endpoint, const FString& Args, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback&
	              OnFailure) const;
	
	const FString SequenceMarketplaceV2Contract = "0xfdb42A198a932C8D3B506Ffa5e855bC4b348a712";
	
	void CheckSardineWhiteListStatus(FString Address, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure);
	
	void SardineGetQuote(TSuccessCallback<FSardineQuote> OnSuccess, const FFailureCallback& OnFailure, FString WalletAddress, FSardineToken Token, u_long Amount, ESardinePaymentType PaymentType = Credit,
		TOptional<FSardineFiatCurrency> QuotedCurrency = TOptional<FSardineFiatCurrency>(),
		ESardineQuoteType QuoteType = Buy);

	void SardineGetClientToken(TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure);

	void OnRamp(FString ClientToken, bool IsDev = false);

	void OnRampAsync(FString ClientToken, TSuccessCallback<void>& OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetNFTCheckoutToken(TArray<FSeqCollectibleOrder> Orders, long Quantity, FString RecipientAddress, TArray<FAdditionalFee> AdditionalFee, FString MarketPlaceContractAddress, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetNFTCheckoutToken(int64 ChainId, UERC1155SaleContract SaleContract, FString CollectionAddress, long TokenID, long Amount, TSuccessCallback
	                                <FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure, FString RecipientAddress = "", TArray<uint8> data =
		                                TArray<uint8>(), TArray<uint8> Proof = TArray<uint8>());

	void SardineGetNFTCheckoutToken(UERC721SaleContract SaleContract, FString CollectionAddress, long TokenID, long Amount, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure, FString RecipientAddress = "", TArray<uint8> data = TArray<uint8>(),TArray<uint8> Proof = TArray<uint8>());

	void SardineGetNFTCheckoutOrderStatus(FString OrderID, TSuccessCallback<FSardineOrder> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetSupportedRegions(TSuccessCallback<TArray<FSardineRegion>> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetSupportedFiatCurrencies(TSuccessCallback<TArray<FSardineFiatCurrency>> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetSupportedTokens(TSuccessCallback<TArray<FSardineSupportedToken>> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetEnabledTokens(TSuccessCallback<TArray<FSardineEnabledToken>> OnSuccess, const FFailureCallback& OnFailure);

	FString CheckoutURL(FString ClientToken, bool IsDev);
	
	FString CheckoutURL(FSardineNFTCheckout Token);

	void Checkout(FSardineNFTCheckout Token);
};

template < typename T> FString USardineCheckout::BuildArgs(T StructIn)
{
	FString Result = "[FAILED TO PARSE]";
	if (StructIn.customGetter)
	{
		Result = StructIn.GetArgs();
	}
	else
	{
		if (!FJsonObjectConverter::UStructToJsonObjectString<T>(StructIn, Result))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert specified UStruct to a json object\n"));
		}
	}
	return Result;
}

//generic
template<typename T> T USardineCheckout::BuildResponse(const FString Text)
{
	//Take the FString and convert it to a JSON object first!
	TSharedPtr<FJsonObject> JSON_Step;
	//Then take the json object we make and convert it to a USTRUCT of type T then we return that!
	T Ret_Struct;

	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), JSON_Step))
	{
		UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *Text);
		return T();
	}
	//this next line with throw an exception in null is used as an entry in json attributes! we need to remove null entries
	if (Ret_Struct.customConstructor)
	{//use the custom constructor!
		Ret_Struct.construct(*JSON_Step.Get());
	}
	else
	{//use unreal parsing!
		if (!FJsonObjectConverter::JsonObjectToUStruct<T>(JSON_Step.ToSharedRef(), &Ret_Struct))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert Json Object: %s to USTRUCT of type T"), *Text);
			return T();
		}
	}
	Ret_Struct.Setup(*JSON_Step.Get());//now for the edge cases we will manually inject the data where it needs to be!
	return Ret_Struct;
}