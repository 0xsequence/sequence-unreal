#include "Subsystems/SequenceCheckoutBP.h"

#include "Sequence/SequenceSdk.h"

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

void USequenceCheckoutBP::GetCheckoutOptions(const FString& WalletAddress,
	const TArray<FCheckoutOptionsMarketplaceOrder>& Orders, const int64 AdditionalFeeBps,
	const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	const TSuccessCallback<FGetCheckoutOptionsResponse> OnApiSuccess = [this, OnSuccess](const FGetCheckoutOptionsResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Checkout->GetCheckoutOptions(SequenceSdk::GetChainId(), WalletAddress, Orders, AdditionalFeeBps, OnApiSuccess, OnApiFailure);
}

void USequenceCheckoutBP::GetCheckoutOptionsByOrders(const FString& WalletAddress, const TArray<FSeqOrder>& Orders,
	const int64 AdditionalFeeBps, const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	const TSuccessCallback<FGetCheckoutOptionsResponse> OnApiSuccess = [this, OnSuccess](const FGetCheckoutOptionsResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Checkout->GetCheckoutOptionsByOrders(SequenceSdk::GetChainId(), WalletAddress, Orders, AdditionalFeeBps, OnApiSuccess, OnApiFailure);
}

void USequenceCheckoutBP::GetCheckoutOptionsByTokenIdAmounts(const FString& WalletAddress,
	const UERC1155SaleContract* SaleContract, const FString& CollectionAddress, const TMap<FString, int64> AmountsByTokenId,
	const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	const TSuccessCallback<FGetCheckoutOptionsResponse> OnApiSuccess = [this, OnSuccess](const FGetCheckoutOptionsResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Checkout->GetCheckoutOptionsByTokenIdAmounts(SequenceSdk::GetChainId(), WalletAddress, SaleContract, CollectionAddress, AmountsByTokenId, OnApiSuccess, OnApiFailure);
}

void USequenceCheckoutBP::GetCheckoutOptionsByERC1155Contract(const FString& WalletAddress,
	UERC1155SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId,
	const int64 Amount, const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	const TSuccessCallback<FGetCheckoutOptionsResponse> OnApiSuccess = [this, OnSuccess](const FGetCheckoutOptionsResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Checkout->GetCheckoutOptionsByERC1155Contract(SequenceSdk::GetChainId(), WalletAddress, SaleContract, CollectionAddress, TokenId, Amount, OnApiSuccess, OnApiFailure);
}

void USequenceCheckoutBP::GetCheckoutOptionsByERC721Contract(const FString& WalletAddress,
	UERC721SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId,
	const int64 Amount, const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	const TSuccessCallback<FGetCheckoutOptionsResponse> OnApiSuccess = [this, OnSuccess](const FGetCheckoutOptionsResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Checkout->GetCheckoutOptionsByERC721Contract(SequenceSdk::GetChainId(), WalletAddress, SaleContract, CollectionAddress, TokenId, Amount, OnApiSuccess, OnApiFailure);
}

void USequenceCheckoutBP::GenerateBuyTransaction(const FString& WalletAddress, const FSeqOrder& Order,
                                                 const int64 Amount, const FAdditionalFee AdditionalFee, const EWalletKind WalletKind,
                                                 const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	const TSuccessCallback<FGenerateTransactionResponse> OnApiSuccess = [this, OnSuccess](const FGenerateTransactionResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Checkout->GenerateBuyTransaction(SequenceSdk::GetChainId(), WalletAddress, Order, Amount, AdditionalFee, WalletKind, OnApiSuccess, OnApiFailure);
}

void USequenceCheckoutBP::GenerateSellTransaction(const FString& WalletAddress, const FSeqOrder& Order,
	const int64 Amount, const FAdditionalFee AdditionalFee, const EWalletKind WalletKind,
	const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	const TSuccessCallback<FGenerateTransactionResponse> OnApiSuccess = [this, OnSuccess](const FGenerateTransactionResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Checkout->GenerateSellTransaction(SequenceSdk::GetChainId(), WalletAddress, Order, Amount, AdditionalFee, WalletKind, OnApiSuccess, OnApiFailure);
}

void USequenceCheckoutBP::GenerateListingTransaction(const FString& WalletAddress, const FString& CollectionAddress,
	const FString& TokenId, const int64 Amount, const EContractType ContractType, const FString& CurrencyTokenAddress,
	const int64 PricePerToken, const FDateTime Expiry, const EOrderbookKind OrderbookKind, const EWalletKind WalletKind,
	const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	const TSuccessCallback<FGenerateTransactionResponse> OnApiSuccess = [this, OnSuccess](const FGenerateTransactionResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Checkout->GenerateListingTransaction(SequenceSdk::GetChainId(), WalletAddress, CollectionAddress, TokenId, Amount, ContractType, CurrencyTokenAddress, PricePerToken, Expiry, OrderbookKind, WalletKind, OnApiSuccess, OnApiFailure);
}

void USequenceCheckoutBP::GenerateOfferTransaction(const FString& WalletAddress, const FString& CollectionAddress,
	const FString& TokenId, const int64 Amount, const EContractType ContractType, const FString& CurrencyTokenAddress,
	const int64 PricePerToken, const FDateTime Expiry, const EOrderbookKind OrderbookKind, const EWalletKind WalletKind,
	const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	const TSuccessCallback<FGenerateTransactionResponse> OnApiSuccess = [this, OnSuccess](const FGenerateTransactionResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Checkout->GenerateOfferTransaction(SequenceSdk::GetChainId(), WalletAddress, CollectionAddress, TokenId, Amount, ContractType, CurrencyTokenAddress, PricePerToken, Expiry, OrderbookKind, WalletKind, OnApiSuccess, OnApiFailure);
}

void USequenceCheckoutBP::GenerateCancelTransaction(const FString& WalletAddress, const FString& CollectionAddress,
	const FString& OrderId, const EMarketplaceKind MarketplaceKind, const FOnGenerateTransactionResponseSuccess OnSuccess,
	const FOnCheckoutFailure OnFailure)
{
	const TSuccessCallback<FGenerateTransactionResponse> OnApiSuccess = [this, OnSuccess](const FGenerateTransactionResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Checkout->GenerateCancelTransaction(SequenceSdk::GetChainId(), WalletAddress, CollectionAddress, OrderId, MarketplaceKind, OnApiSuccess, OnApiFailure);
}

void USequenceCheckoutBP::GenerateCancelTransactionByOrder(const FString& WalletAddress,
	const FString& CollectionAddress, const FSeqOrder& Order, const EMarketplaceKind MarketplaceKind,
	const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure)
{
	const TSuccessCallback<FGenerateTransactionResponse> OnApiSuccess = [this, OnSuccess](const FGenerateTransactionResponse& Response)
	{
		OnSuccess.ExecuteIfBound(Response);
	};

	const FFailureCallback OnApiFailure = [this, OnFailure](const FSequenceError& Error)
	{
		SEQ_LOG(Error, TEXT("Email Auth Error: %s"), *Error.Message);
		OnFailure.ExecuteIfBound(Error.Message);
	};
	
	this->Checkout->GenerateCancelTransactionByOrder(SequenceSdk::GetChainId(), WalletAddress, CollectionAddress, Order, MarketplaceKind, OnApiSuccess, OnApiFailure);
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

