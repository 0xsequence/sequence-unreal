#include "Integrators/SequenceCheckoutBP.h"

void USequenceCheckoutBP::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Warning, TEXT("SequenceCheckoutBP::Initialize"));
}

void USequenceCheckoutBP::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Warning, TEXT("SequenceCheckoutBP::Deinitialize"));
}

USequenceCheckoutBP::USequenceCheckoutBP()
{
	UE_LOG(LogTemp, Warning, TEXT("SequenceCheckoutBP::Constructor"));
	
	this->Checkout = NewObject<UCheckout>();
}

void USequenceCheckoutBP::SetChainID(const int64& InChainID)
{
	this->Checkout->SetChainID(InChainID);
}

void USequenceCheckoutBP::GenerateListingTransaction(const FString& WalletAddress, const FString& CollectionAddress,
                                                     const FString& TokenId, const int64 Amount, const EContractType ContractType, const FString& CurrencyTokenAddress,
                                                     const int64 PricePerToken, const FDateTime Expiry, const EOrderbookKind OrderbookKind, const EWalletKind WalletKind,
                                                     const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure) const
{
	this->Checkout->GenerateListingTransaction(WalletAddress, CollectionAddress, TokenId, Amount, ContractType, CurrencyTokenAddress, PricePerToken, Expiry, OrderbookKind, WalletKind, OnSuccess, OnFailure);
}

// UCheckout* USequenceCheckoutBP::GetCheckout() const
// {
// 	if (this->Checkout == nullptr)
// 	{
// 		this->Checkout = NewObject<UCheckout>();
// 	}
// 	
// 	return this->Checkout;
// }
