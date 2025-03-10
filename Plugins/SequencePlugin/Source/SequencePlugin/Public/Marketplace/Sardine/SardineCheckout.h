// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Marketplace/Structs/AdditionalFee.h"
#include "Marketplace/Structs/SeqCollectibleOrder.h"
#include "Structs/SardineFiatCurrency.h"
#include "Structs/SardineNFTCheckout.h"
#include "Structs/SardinePaymentType.h"
#include "Structs/SardineQuote.h"
#include "Structs/SardineQuoteType.h"
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
	void CheckSardineWhiteListStatus(FString Address, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure);
	
	void SardineGetQuote(FSardineToken Token, u_long Amount, ESardinePaymentType PaymentType = Credit,
		TOptional<FSardineFiatCurrency> QuotedCurrency = TOptional<FSardineFiatCurrency>(),
		ESardineQuoteType QuoteType = Buy, TSuccessCallback<FSardineQuote> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetClientToken(TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure);

	void OnRamp(FString ClientToken);

	void OnRampAsync(FString ClientToken, TFunction<void> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetNFTCheckoutToken(TArray<FSeqCollectibleOrder> Orders, u_long Quantity, FString RecipientAddress, TArray<FAdditionalFee> AdditionalFee, FString MarketPlaceContractAddress, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetNFTCheckoutToken(UERC1155SaleContract SaleContract, u_long Quantity, FString RecipientAddress, TArray<FAdditionalFee> AdditionalFee, FString MarketPlaceContractAddress, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetNFTCheckoutToken(UERC721SaleContract SaleContract, u_long Quantity, FString RecipientAddress, TArray<FAdditionalFee> AdditionalFee, FString MarketPlaceContractAddress, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure);

	void SardineGetNFTCheckoutOrderStatus(FString OrderID, TSuccessCallback<SardineOrder> OnSuccess, const FFailureCallback& OnFailure);
	
	FString CheckoutURL(FSardineNFTCheckout Token);

	void Checkout(FSardineNFTCheckout Token);
};
