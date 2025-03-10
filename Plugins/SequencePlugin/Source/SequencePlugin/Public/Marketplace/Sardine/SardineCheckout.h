// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Marketplace/Structs/AdditionalFee.h"
#include "Marketplace/Structs/SeqCollectibleOrder.h"
#include "Structs/SardineEnabledToken.h"
#include "Structs/SardineFiatCurrency.h"
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

public:
	const FString SequenceMarketplaceV2Contract = "0xfdb42A198a932C8D3B506Ffa5e855bC4b348a712";
	
	void CheckSardineWhiteListStatus(FString Address, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure);
	
	void SardineGetQuote(FSardineToken Token, u_long Amount, ESardinePaymentType PaymentType = Credit,
		TOptional<FSardineFiatCurrency> QuotedCurrency = TOptional<FSardineFiatCurrency>(),
		ESardineQuoteType QuoteType = Buy, TSuccessCallback<FSardineQuote> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetClientToken(TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure);

	void OnRamp(FString ClientToken);

	void OnRampAsync(FString ClientToken, TFunction<void> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetNFTCheckoutToken(TArray<FSeqCollectibleOrder> Orders, u_long Quantity, FString RecipientAddress, TArray<FAdditionalFee> AdditionalFee, FString MarketPlaceContractAddress, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetNFTCheckoutToken(UERC1155SaleContract SaleContract, FString CollectionAddress, u_long TokenID, u_long Amount, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure, FString RecipientAddress = "", TArray<uint8> data = TArray<uint8>(),TArray<uint8> Proof = TArray<uint8>());

	void SardineGetNFTCheckoutToken(UERC721SaleContract SaleContract, FString CollectionAddress, u_long TokenID, u_long Amount, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure, FString RecipientAddress = "", TArray<uint8> data = TArray<uint8>(),TArray<uint8> Proof = TArray<uint8>());

	void SardineGetNFTCheckoutOrderStatus(FString OrderID, TSuccessCallback<FSardineOrder> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetSupportedRegions(TSuccessCallback<TArray<FSardineRegion>> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetSupportedFiatCurrencies(TSuccessCallback<TArray<FSardineFiatCurrency>> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetSupportedTokens(TSuccessCallback<TArray<FSardineSupportedToken>> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetEnabledTokens(TSuccessCallback<TArray<FSardineEnabledToken>> OnSuccess, const FFailureCallback& OnFailure);
	
	FString CheckoutURL(FSardineNFTCheckout Token);

	void Checkout(FSardineNFTCheckout Token);
};
