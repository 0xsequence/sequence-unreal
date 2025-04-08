#pragma once

#include "CoreMinimal.h"
#include "Checkout/SequenceCheckout.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sequence/SequenceResponseIntent.h"
#include "Marketplace/Sardine/SardineCheckout.h"
#include "SardineCheckoutBP.generated.h"

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCheckSardineWhiteListStatus, bool, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSardineGetQuote, FSardineQuote, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSardineGetClientToken, FString, Response);
DECLARE_DYNAMIC_DELEGATE(FOnRampAsync);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSardineGetNFTCheckoutToken, FSardineNFTCheckout, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSardineGetNFTCheckoutOrderStatus, FSardineOrder, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSardineGetSupportedRegions, TArray<FSardineRegion>, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSardineGetSupportedFiatCurrencies, TArray<FSardineFiatCurrency>, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSardineGetSupportedTokens, TArray<FSardineSupportedToken>, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSardineGetEnabledTokens, TArray<FSardineEnabledToken>, Response);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE(FOnSardineCheckoutFailure);

UCLASS()
class SEQUENCEPLUGIN_API USardineCheckoutBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	USardineCheckout* SardineCheckout;
	
public:
	USardineCheckoutBP();

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void CheckSardineWhiteListStatus(FString Address, FOnCheckSardineWhiteListStatus OnSuccess, FOnSardineCheckoutFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetQuote(FOnSardineGetQuote OnSuccess, FOnSardineCheckoutFailure OnFailure, FString WalletAddress, FSardineToken Token, int Amount, FSardineFiatCurrency QuotedCurrency, ESardinePaymentType PaymentType = Credit, ESardineQuoteType QuoteType = Buy);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetClientToken(FOnSardineGetClientToken OnSuccess, FOnSardineCheckoutFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void OnRamp(const FString& ClientToken);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetNFTCheckoutTokenOrders(int64 ChainId, const FString& WalletAddress, TArray<FSeqCollectibleOrder> Orders, int Quantity, FString RecipientAddress, TArray
	                                <FAdditionalFee> AdditionalFee, FString MarketPlaceContractAddress, FOnSardineGetNFTCheckoutToken OnSuccess, FOnSardineCheckoutFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetNFTCheckoutTokenERC1155(int64 ChainId, UERC1155SaleContract* SaleContract, FString CollectionAddress, int TokenID, int Amount,
		FOnSardineGetNFTCheckoutToken OnSuccess, FOnSardineCheckoutFailure OnFailure,
		TArray<uint8> data, TArray<FString> Proof, FString RecipientAddress = "");

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetNFTCheckoutTokenERC721(int64 ChainID, UERC721SaleContract* SaleContract, FString CollectionAddress, int TokenID,
		int Amount, FOnSardineGetNFTCheckoutToken OnSuccess, FOnSardineCheckoutFailure OnFailure, FString RecipientAddress, TArray<uint8> data , TArray<FString> Proof);
	
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetNFTCheckoutOrderStatus(FString OrderID, FOnSardineGetNFTCheckoutOrderStatus OnSuccess, FOnSardineCheckoutFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetSupportedRegions(FOnSardineGetSupportedRegions OnSuccess, FOnSardineCheckoutFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetSupportedFiatCurrencies(FOnSardineGetSupportedFiatCurrencies OnSuccess, FOnSardineCheckoutFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetSupportedTokens(FOnSardineGetSupportedTokens OnSuccess, FOnSardineCheckoutFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetEnabledTokens(FOnSardineGetEnabledTokens OnSuccess, FOnSardineCheckoutFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	FString CheckoutURL(FString ClientToken);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	FString CheckoutURLFromToken(FSardineNFTCheckout Token);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void Checkout(FSardineNFTCheckout Token);
	
};
