// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once
#include "Errors.h"
#include "TransakDataTypes.h"
#include "Util/Async.h"

class SequenceAPITest
{
public:
	static void BasicProviderTests();
	static void SignMessage(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void ListSessions(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void SendRaw(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void CallContract(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void SendERC20(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void SendERC721(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void SendERC1155(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void CloseSession(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void GetFeeOptions(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void SendTransactionWithFee(TFunction<void(FString)> OnSuccess, TFunction<void(FString,FSequenceError)> OnFailure);
	static void GetUnfilteredFeeOptions(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void GetSupportedCountries(const TSuccessCallback<TArray<FSupportedCountry>>& OnSuccess, const FFailureCallback& OnFailure);
	static void TestLoadTransakUrl();

	// Market
	static void ListCurrencies(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void ListAllCollectibleListingsWithLowestPriceListingsFirst(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void ListAllCollectibleOffersWithHighestPricedOfferFirst(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void GetLowestPriceOfferForCollectible(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void GetHighestPriceOfferForCollectible(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void GetLowestPriceListingForCollectible(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void GetHighestPriceListingForCollectible(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void ListAllListingsForCollectible(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void ListAllOffersForCollectible(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	static void GetFloorOrder(TFunction<void(FString)> OnSuccess, TFunction<void(FString, FSequenceError)> OnFailure);
	
};
