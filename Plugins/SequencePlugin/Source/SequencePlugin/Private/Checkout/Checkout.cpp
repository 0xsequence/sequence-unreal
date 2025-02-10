#include "Checkout.h"

#include "Checkout/Structs/GenerateBuyTransaction.h"
#include "Checkout/Structs/GenerateCancelTransactionRequest.h"
#include "Checkout/Structs/GenerateListingTransactionArgs.h"
#include "Checkout/Structs/GenerateOfferTransactionArgs.h"
#include "Checkout/Structs/GenerateSellTransaction.h"
#include "Checkout/Structs/OrderData.h"

UCheckout::UCheckout(): ChainID()
{;
	this->ChainID = 0;
}

UCheckout::UCheckout(UMarketplace* Marketplace, USequenceWallet* InWallet, const int64& InChainID):
	ChainID()
{
	this->Marketplace = Marketplace;
	this->Wallet = InWallet;
	this->ChainID = InChainID;
}

template < typename T> FString UCheckout::BuildArgs(T StructIn) const
{
	FString Result = "[FAILED TO PARSE]";
	if (StructIn.customGetter)
	{
		Result = StructIn.GetArgs();
	}
	else
	{
		if (!FJsonObjectConverter::UStructToJsonObjectString<T>(StructIn, Result))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert specified UStruct to a json object\n"));
		}
	}
	return Result;
}


template<typename T> T UCheckout::BuildResponse(const FString Text) const
{
	//Take the FString and convert it to a JSON object first!
	TSharedPtr<FJsonObject> JSON_Step;
	//Then take the json object we make and convert it to a USTRUCT of type T then we return that!
	T Ret_Struct;

	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), JSON_Step))
	{
		UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *Text);
		return T();
	}
	//this next line with throw an exception in null is used as an entry in json attributes! we need to remove null entries
	if (Ret_Struct.customConstructor)
	{//use the custom constructor!
		Ret_Struct.Construct(*JSON_Step.Get());
	}
	else
	{//use unreal parsing!
		if (!FJsonObjectConverter::JsonObjectToUStruct<T>(JSON_Step.ToSharedRef(), &Ret_Struct))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert Json Object: %s to USTRUCT of type T"), *Text);
			return T();
		}
	}
	Ret_Struct.Setup(*JSON_Step.Get());//now for the edge cases we will manually inject the data where it needs to be!
	return Ret_Struct;
}

void UCheckout::GenerateBuyTransaction(const FSeqOrder& Order, const int64 Amount, const FAdditionalFee& AdditionalFee,
	TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	TArray<FOrderData> OrdersData;
	OrdersData.Reserve(1);
	OrdersData.Add(FOrderData(Order.OrderId, FString::Printf(TEXT("%lld"), Amount)));

	const TArray AdditionalFees { AdditionalFee };

	const FString Endpoint = "GenerateBuyTransaction";
	const FString Args = BuildArgs<FGenerateBuyTransaction>(FGenerateBuyTransaction{ Order.CollectionContractAddress, Wallet->GetWalletAddress(), Order.Marketplace, OrdersData, AdditionalFees, UMarketplaceWalletKindExtensions::GetWalletKind(Wallet) });
	
	Marketplace->HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
		{
			const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

void UCheckout::GenerateSellTransaction(const FSeqOrder& Order, int64 Amount, const FAdditionalFee& AdditionalFee,
	TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	TArray<FOrderData> OrdersData;
	OrdersData.Reserve(1);
	OrdersData.Add(FOrderData(Order.OrderId, FString::Printf(TEXT("%lld"), Amount)));

	const TArray AdditionalFees { AdditionalFee };

	const FString Endpoint = "GenerateSellTransaction";
	const FString Args = BuildArgs<FGenerateSellTransaction>(FGenerateSellTransaction{ Order.CollectionContractAddress, Wallet->GetWalletAddress(), Order.Marketplace, OrdersData, AdditionalFees, UMarketplaceWalletKindExtensions::GetWalletKind(Wallet) });;

	Marketplace->HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
		{
			const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

void UCheckout::GenerateListingTransaction(const FString& CollectionAddress, const FString& TokenId, const int64 Amount, const EContractType ContractType,
	const FString& CurrencyTokenAddress, const int64 PricePerToken, const FDateTime Expiry, const EOrderbookKind OrderbookKind,
	TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	int64 EpochTime = Expiry.ToUnixTimestamp();
	const FString Endpoint = "GenerateListingTransaction";
	const FString Args = BuildArgs<FGenerateListingTransactionArgs>(FGenerateListingTransactionArgs {
		CollectionAddress, Wallet->GetWalletAddress(), ContractType, OrderbookKind, FCreateReq {
		TokenId, FString::Printf(TEXT("%lld"), Amount), FString::Printf(TEXT("%lld"), EpochTime), CurrencyTokenAddress, FString::Printf(TEXT("%lld"), PricePerToken)
		},  UMarketplaceWalletKindExtensions::GetWalletKind(Wallet)
	});

	Marketplace->HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
		{
			const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

void UCheckout::GenerateOfferTransaction(const FString& CollectionAddress, const FString& TokenId, const int64 Amount,
	const EContractType ContractType, const FString& CurrencyTokenAddress, const int64 PricePerToken, const FDateTime Expiry,
	const EOrderbookKind OrderbookKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess,
	const FFailureCallback& OnFailure) const
{
	int64 EpochTime = Expiry.ToUnixTimestamp();
	const FString Endpoint = "GenerateOfferTransaction";
	const FString Args = BuildArgs<FGenerateOfferTransactionArgs>(FGenerateOfferTransactionArgs {
		CollectionAddress, Wallet->GetWalletAddress(), ContractType, OrderbookKind, FCreateReq {
		TokenId, FString::Printf(TEXT("%lld"), Amount), FString::Printf(TEXT("%lld"), EpochTime), CurrencyTokenAddress, FString::Printf(TEXT("%lld"), PricePerToken)
		},  UMarketplaceWalletKindExtensions::GetWalletKind(Wallet)
	});

	Marketplace->HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
		{
			const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

void UCheckout::GenerateCancelTransaction(const FString& CollectionAddress, const FString& OrderId,
	const EMarketplaceKind MarketplaceKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess,
	const FFailureCallback& OnFailure) const
{
	const FString Endpoint = "GenerateCancelTransaction";
	const FString Args = BuildArgs<FGenerateCancelTransactionRequest>(FGenerateCancelTransactionRequest {
		CollectionAddress, Wallet->GetWalletAddress(), MarketplaceKind, OrderId
	});

	Marketplace->HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
		{
			const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

void UCheckout::GenerateCancelTransaction(const FString& CollectionAddress, const FSeqOrder& Order,
	const EMarketplaceKind MarketplaceKind, const TSuccessCallback<FGenerateTransactionResponse>& OnSuccess,
	const FFailureCallback& OnFailure) const
{
	GenerateCancelTransaction(CollectionAddress, Order.OrderId, MarketplaceKind, OnSuccess, OnFailure);
}
