#pragma once

#include "CoreMinimal.h"
#include "Checkout/SequenceCheckout.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sequence/SequenceResponseIntent.h"
#include "Marketplace/Sardine/SardineCheckout.h"
#include "SardineCheckoutBP.generated.h"

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCheckSardineWhiteListStatus, bool, Response);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSardineGetClientToken, FString, Response);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnSardineCheckoutFailure, FSequenceError, Error);

UCLASS()
class SEQUENCEPLUGIN_API USardineCheckoutBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	USardineCheckout* SardineCheckout;
	
public:
	USardineCheckoutBP();

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void CheckSardineWhiteListStatus(FString Address, FOnCheckSardineWhiteListStatus OnSuccess, FOnSardineCheckoutFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetQuote(TSuccessCallback<FSardineQuote> OnSuccess, const FFailureCallback& OnFailure, FString WalletAddress, FSardineToken Token, u_long Amount, ESardinePaymentType PaymentType = Credit,
		TOptional<FSardineFiatCurrency> QuotedCurrency = TOptional<FSardineFiatCurrency>(),
		ESardineQuoteType QuoteType = Buy);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetClientToken(FOnSardineGetClientToken OnSuccess, FOnSardineCheckoutFailure OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void OnRamp(const FString& ClientToken);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void OnRampAsync(FString ClientToken, TFunction<void> OnSuccess, const FFailureCallback& OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetNFTCheckoutToken(int64 ChainId, const FString& WalletAddress, TArray<FSeqCollectibleOrder> Orders, long Quantity, FString RecipientAddress, TArray
	                                <FAdditionalFee> AdditionalFee, FString MarketPlaceContractAddress, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const
	                                FFailureCallback& OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetNFTCheckoutToken(int64 ChainId, UERC1155SaleContract* SaleContract, FString CollectionAddress, long TokenID, long Amount,
		TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure, FString RecipientAddress = "",
		TArray<uint8> data = TArray<uint8>(), TArray<FString> Proof = TArray<FString>());

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetNFTCheckoutToken(int64 ChainId, UERC721SaleContract SaleContract, FString CollectionAddress, long TokenID, long Amount, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure, FString RecipientAddress = "", TArray<uint8> data = TArray<uint8>(),TArray<uint8> Proof = TArray<uint8>());

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetNFTCheckoutOrderStatus(FString OrderID, TSuccessCallback<FSardineOrder> OnSuccess, const FFailureCallback& OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetSupportedRegions(TSuccessCallback<TArray<FSardineRegion>> OnSuccess, const FFailureCallback& OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetSupportedFiatCurrencies(TSuccessCallback<TArray<FSardineFiatCurrency>> OnSuccess, const FFailureCallback& OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetSupportedTokens(TSuccessCallback<TArray<FSardineSupportedToken>> OnSuccess, const FFailureCallback& OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SardineGetEnabledTokens(TSuccessCallback<TArray<FSardineEnabledToken>> OnSuccess, const FFailureCallback& OnFailure);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	FString CheckoutURL(FString ClientToken);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	FString CheckoutURL(FSardineNFTCheckout Token);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void Checkout(FSardineNFTCheckout Token);
	
};
