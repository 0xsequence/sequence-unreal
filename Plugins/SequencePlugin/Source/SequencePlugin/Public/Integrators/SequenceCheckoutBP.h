#pragma once

#include "CoreMinimal.h"
#include "Checkout/SequenceCheckout.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sequence/SequenceResponseIntent.h"
#include "SequenceCheckoutBP.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API USequenceCheckoutBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
public:
	USequenceCheckoutBP();

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SetChainID(const int64& InChainID);

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptions(const FString& WalletAddress, const TArray<FCheckoutOptionsMarketplaceOrder>& Orders, int64 AdditionalFeeBps, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptionsByOrders(const FString& WalletAddress, const TArray<FSeqOrder>& Orders, const int64 AdditionalFeeBps, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptionsByTokenIdAmounts(const FString& WalletAddress, const UERC1155SaleContract* SaleContract, const FString& CollectionAddress, TMap<FString, int64> AmountsByTokenId, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptionsByERC1155Contract(const FString& WalletAddress, UERC1155SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId, const int64 Amount, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GetCheckoutOptionsByERC721Contract(const FString& WalletAddress, UERC721SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId, const int64 Amount, FOnGetCheckoutOptionsResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
	
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateBuyTransaction(const FString& WalletAddress, const FSeqOrder& Order, const int64 Amount, const FAdditionalFee AdditionalFee, const EWalletKind WalletKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateSellTransaction(const FString& WalletAddress, const FSeqOrder& Order, const int64 Amount, const FAdditionalFee AdditionalFee, const EWalletKind WalletKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateListingTransaction(const FString& WalletAddress, const FString& CollectionAddress, const FString& TokenId, int64 Amount, EContractType ContractType, const FString& CurrencyTokenAddress, int64 PricePerToken, FDateTime Expiry, EOrderbookKind OrderbookKind, const EWalletKind WalletKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateOfferTransaction(const FString& WalletAddress, const FString& CollectionAddress, const FString& TokenId, int64 Amount, EContractType ContractType, const FString& CurrencyTokenAddress, int64 PricePerToken, FDateTime Expiry, EOrderbookKind OrderbookKind, const EWalletKind WalletKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateCancelTransaction(const FString& WalletAddress, const FString& CollectionAddress, const FString& OrderId, EMarketplaceKind MarketplaceKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateCancelTransactionByOrder(const FString& WalletAddress, const FString& CollectionAddress, const FSeqOrder& Order, const EMarketplaceKind MarketplaceKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure);
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	UTransactions* StepsToTransactions(const TArray<FTransactionStep>& Steps);
	
private:
	USequenceCheckout* Checkout;

	UTransactions* CreateTransaction();
};
