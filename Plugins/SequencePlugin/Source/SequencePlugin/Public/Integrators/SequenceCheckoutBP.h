#pragma once

#include "CoreMinimal.h"
#include "Checkout/Checkout.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SequenceCheckoutBP.generated.h"

UCLASS()
class SEQUENCEPLUGIN_API USequenceCheckoutBP : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
public:
	USequenceCheckoutBP();
	// UFUNCTION(BlueprintCallable, Category = "0xSequence SDK/Checkout")
	// UCheckout* GetCheckout() const;

	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void SetChainID(const int64& InChainID);
	
	UFUNCTION(BlueprintCallable, Category = "Checkout")
	void GenerateListingTransaction(const FString& WalletAddress, const FString& CollectionAddress, const FString& TokenId, int64 Amount, EContractType ContractType, const FString& CurrencyTokenAddress, int64 PricePerToken, FDateTime Expiry, EOrderbookKind OrderbookKind, const EWalletKind WalletKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const;

private:
	UCheckout* Checkout;
	
};
