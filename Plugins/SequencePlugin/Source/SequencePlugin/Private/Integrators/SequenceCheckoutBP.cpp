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
	
	this->Checkout = NewObject<USequenceCheckout>();
}

// Do NOT add const for this function, breaks how we get the chainID
void USequenceCheckoutBP::SetChainID(const int64& InChainID)
{
	this->Checkout->SetChainID(InChainID);
}

void USequenceCheckoutBP::GetCheckoutOptions(const FString& WalletAddress,
	const TArray<FCheckoutOptionsMarketplaceOrder>& Orders, const int64 AdditionalFeeBps,
	const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	this->Checkout->GetCheckoutOptions(WalletAddress, Orders, AdditionalFeeBps, OnSuccess, OnFailure);
}

void USequenceCheckoutBP::GetCheckoutOptionsByOrders(const FString& WalletAddress, const TArray<FSeqOrder>& Orders,
	const int64 AdditionalFeeBps, const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	this->Checkout->GetCheckoutOptionsByOrders(WalletAddress, Orders, AdditionalFeeBps, OnSuccess, OnFailure);
}

void USequenceCheckoutBP::GetCheckoutOptionsByTokenIdAmounts(const FString& WalletAddress,
	const UERC1155SaleContract* SaleContract, const FString& CollectionAddress, const TMap<FString, int64> AmountsByTokenId,
	const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	this->Checkout->GetCheckoutOptionsByTokenIdAmounts(WalletAddress, SaleContract, CollectionAddress, AmountsByTokenId, OnSuccess, OnFailure);
}

void USequenceCheckoutBP::GetCheckoutOptionsByERC1155Contract(const FString& WalletAddress,
	UERC1155SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId,
	const int64 Amount, const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	this->Checkout->GetCheckoutOptionsByERC1155Contract(WalletAddress, SaleContract, CollectionAddress, TokenId, Amount, OnSuccess, OnFailure);
}

void USequenceCheckoutBP::GetCheckoutOptionsByERC721Contract(const FString& WalletAddress,
	UERC721SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId,
	const int64 Amount, const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	this->Checkout->GetCheckoutOptionsByERC721Contract(WalletAddress, SaleContract, CollectionAddress, TokenId, Amount, OnSuccess, OnFailure);
}

void USequenceCheckoutBP::GenerateBuyTransaction(const FString& WalletAddress, const FSeqOrder& Order,
                                                 const int64 Amount, const FAdditionalFee AdditionalFee, const EWalletKind WalletKind,
                                                 const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	this->Checkout->GenerateBuyTransaction(WalletAddress, Order, Amount, AdditionalFee, WalletKind, OnSuccess, OnFailure);
}

void USequenceCheckoutBP::GenerateSellTransaction(const FString& WalletAddress, const FSeqOrder& Order,
	const int64 Amount, const FAdditionalFee AdditionalFee, const EWalletKind WalletKind,
	const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	this->Checkout->GenerateSellTransaction(WalletAddress, Order, Amount, AdditionalFee, WalletKind, OnSuccess, OnFailure);
}

void USequenceCheckoutBP::GenerateListingTransaction(const FString& WalletAddress, const FString& CollectionAddress,
	const FString& TokenId, const int64 Amount, const EContractType ContractType, const FString& CurrencyTokenAddress,
	const int64 PricePerToken, const FDateTime Expiry, const EOrderbookKind OrderbookKind, const EWalletKind WalletKind,
	const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	this->Checkout->GenerateListingTransaction(WalletAddress, CollectionAddress, TokenId, Amount, ContractType, CurrencyTokenAddress, PricePerToken, Expiry, OrderbookKind, WalletKind, OnSuccess, OnFailure);
}

void USequenceCheckoutBP::GenerateOfferTransaction(const FString& WalletAddress, const FString& CollectionAddress,
	const FString& TokenId, const int64 Amount, const EContractType ContractType, const FString& CurrencyTokenAddress,
	const int64 PricePerToken, const FDateTime Expiry, const EOrderbookKind OrderbookKind, const EWalletKind WalletKind,
	const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	this->Checkout->GenerateOfferTransaction(WalletAddress, CollectionAddress, TokenId, Amount, ContractType, CurrencyTokenAddress, PricePerToken, Expiry, OrderbookKind, WalletKind, OnSuccess, OnFailure);
}

void USequenceCheckoutBP::GenerateCancelTransaction(const FString& WalletAddress, const FString& CollectionAddress,
	const FString& OrderId, const EMarketplaceKind MarketplaceKind, const FOnGenerateTransactionResponseSuccess OnSuccess,
	const FOnCheckoutFailure OnFailure)
{
	this->Checkout->GenerateCancelTransaction(WalletAddress, CollectionAddress, OrderId, MarketplaceKind, OnSuccess, OnFailure);
}

void USequenceCheckoutBP::GenerateCancelTransactionByOrder(const FString& WalletAddress,
	const FString& CollectionAddress, const FSeqOrder& Order, const EMarketplaceKind MarketplaceKind,
	const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	this->Checkout->GenerateCancelTransactionByOrder(WalletAddress, CollectionAddress, Order, MarketplaceKind, OnSuccess, OnFailure);
}

UTransactions* USequenceCheckoutBP::StepsToTransactions(const TArray<FTransactionStep>& Steps)
{
	UTransactions* Transactions = CreateTransaction();

	for (const FTransactionStep& Step : Steps)
	{		
		FRawTransaction RawTransaction = FRawTransaction(Step.To, Step.Data, Step.Value.IsEmpty() ? TEXT("0") : Step.Value);
		
		Transactions->AddRaw(RawTransaction);
	}
	
	return Transactions;
}

UTransactions* USequenceCheckoutBP::CreateTransaction()
{
	return NewObject<UTransactions>(this);
}

