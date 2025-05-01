#include "Checkout/SequenceCheckout.h"
#include "ConfigFetcher.h"
#include "Checkout/Structs/GenerateBuyTransaction.h"
#include "Checkout/Structs/GenerateCancelTransactionRequest.h"
#include "Checkout/Structs/GenerateListingTransactionArgs.h"
#include "Checkout/Structs/GenerateOfferTransactionArgs.h"
#include "Checkout/Structs/GenerateSellTransaction.h"
#include "Checkout/Structs/GetCheckoutOptionsArgs.h"
#include "Checkout/Structs/GetCheckoutOptionsResponse.h"
#include "Checkout/Structs/GetPrimarySaleCheckoutOptionsArgs.h"
#include "Checkout/Structs/OrderData.h"
#include "Util/Log.h"

FString USequenceCheckout::Url(const int64& TargetChainID, const FString& EndPoint) const
{
	FString Out_URL = HostName(TargetChainID);
	Out_URL.Append(this->Path);

	if (EndPoint.Len() > 0)
	{
		Out_URL.Append(EndPoint);
	}
	return Out_URL;
}

FString USequenceCheckout::HostName(int64 TargetChainID)
{
	FString Hostname = "https://";
	Hostname.Append("marketplace-api.sequence.app/");
	Hostname.Append(USequenceSupport::GetNetworkNameForUrl(TargetChainID));
	return Hostname;
}

void USequenceCheckout::HTTPPost(const int64& TargetChainID, const FString& Endpoint, const FString& Args,
                         const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString RequestURL = this->Url(TargetChainID, Endpoint);

	const TSharedRef<IHttpRequest> HTTP_Post_Req = FHttpModule::Get().CreateRequest();

	const FString AccessKey = UConfigFetcher::GetConfigVar("ProjectAccessKey");
	if (AccessKey.IsEmpty())
	{
		SEQ_LOG(Error, TEXT("AccessKey is empty! Failed to set HTTP header."));
		return;  
	}

	HTTP_Post_Req->SetVerb("POST");
	HTTP_Post_Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HTTP_Post_Req->SetHeader(TEXT("Accept"), TEXT("application/json"));
	

	HTTP_Post_Req->SetHeader(TEXT("X-Access-Key"), *AccessKey);	
	HTTP_Post_Req->SetTimeout(30);
	HTTP_Post_Req->SetURL(RequestURL);
	HTTP_Post_Req->SetContentAsString(Args);

	const FString CurlCommand = FString::Printf(
		TEXT("curl -X %s \"%s\" -H \"Content-Type: application/json\" -H \"Accept: application/json\" -H \"X-Access-Key: %s\" --data \"%s\""),
		*HTTP_Post_Req->GetVerb(),
		*HTTP_Post_Req->GetURL(),
		*HTTP_Post_Req->GetHeader("X-Access-Key"),
		*FString(UTF8_TO_TCHAR(HTTP_Post_Req->GetContent().GetData())).Replace(TEXT("\""), TEXT("\\\""))
	);

	SEQ_LOG_EDITOR(Log, TEXT("%s"), *CurlCommand);


	HTTP_Post_Req->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](const FHttpRequestPtr& Request, FHttpResponsePtr Response, const bool bWasSuccessful)
		{
			if (bWasSuccessful)
			{
				const FString Content = Response->GetContentAsString();
				SEQ_LOG(Display, TEXT("Response: %s"), *Content);  
				OnSuccess(Content);
			}
			else
			{
				if (Request.IsValid() && Response.IsValid())
				{
					const FString ErrorMessage = Response->GetContentAsString();
					SEQ_LOG(Error, TEXT("Request failed: %s"), *ErrorMessage);  
					OnFailure(FSequenceError(RequestFail, "Request failed: " + ErrorMessage));
				}
				else
				{
					SEQ_LOG(Error, TEXT("Request failed: Invalid Request Pointer")); 
					OnFailure(FSequenceError(RequestFail, "Request failed: Invalid Request Pointer"));
				}
			}
		});

	// Process the request
	HTTP_Post_Req->ProcessRequest();
}

template < typename T> FString USequenceCheckout::BuildArgs(T StructIn) const
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
			SEQ_LOG(Display, TEXT("Failed to convert specified UStruct to a json object\n"));
		}
	}
	return Result;
}


template<typename T> T USequenceCheckout::BuildResponse(const FString Text) const
{
	//Take the FString and convert it to a JSON object first!
	TSharedPtr<FJsonObject> JSON_Step;
	//Then take the json object we make and convert it to a USTRUCT of type T then we return that!
	T Ret_Struct;

	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), JSON_Step))
	{
		SEQ_LOG(Display, TEXT("Failed to convert String: %s to Json object"), *Text);
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
			SEQ_LOG(Display, TEXT("Failed to convert Json Object: %s to USTRUCT of type T"), *Text);
			return T();
		}
	}
	Ret_Struct.Setup(*JSON_Step.Get());//now for the edge cases we will manually inject the data where it needs to be!
	return Ret_Struct;
}

void USequenceCheckout::GetCheckoutOptions(const int64 ChainID, const FString& WalletAddress, const TArray<FCheckoutOptionsMarketplaceOrder>& Orders, const int64 AdditionalFeeBps,
                                   TSuccessCallback<FGetCheckoutOptionsResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	if (Orders.IsEmpty())
	{
		SEQ_LOG(Error, TEXT("Orders is empty"));
		return;
	}
	
	const FString Endpoint = "CheckoutOptionsMarketplace";
	const FString Args = BuildArgs<FGetCheckoutOptionsArgs>(FGetCheckoutOptionsArgs { WalletAddress, Orders, AdditionalFeeBps });

	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGetCheckoutOptionsResponse Response = BuildResponse<FGetCheckoutOptionsResponse>(Content);
		OnSuccess(Response);
	},
	OnFailure);
}

void USequenceCheckout::GetCheckoutOptionsByOrders(const int64 ChainID, const FString& WalletAddress, const TArray<FSeqOrder>& Orders, const int64 AdditionalFeeBps,
	const TSuccessCallback<FGetCheckoutOptionsResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	TArray<FCheckoutOptionsMarketplaceOrder> Options;
	
	for (int32 i = 0; i < Orders.Num(); i++)
	{
		Options.Add(FCheckoutOptionsMarketplaceOrder(Orders[i].CollectionContractAddress, Orders[i].OrderId, Orders[i].Marketplace));
	}

	GetCheckoutOptions(ChainID, WalletAddress, Options, AdditionalFeeBps, OnSuccess, OnFailure);
}

void USequenceCheckout::GetCheckoutOptionsByTokenIdAmounts(const int64 ChainID, const FString& WalletAddress,
	const UERC1155SaleContract* SaleContract, const FString& CollectionAddress, const TMap<FString, int64> AmountsByTokenId,
	TSuccessCallback<FGetCheckoutOptionsResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString Endpoint = "CheckoutOptionsSalesContract";
	const FString Args = BuildArgs<FGetPrimarySaleCheckoutOptionsArgs>(FGetPrimarySaleCheckoutOptionsArgs { WalletAddress, SaleContract->ContractAddress, CollectionAddress, AmountsByTokenId });

	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGetCheckoutOptionsResponse Response = BuildResponse<FGetCheckoutOptionsResponse>(Content);
		OnSuccess(Response);
	},
	[this, OnFailure](const FSequenceError& Error)
	{
		OnFailure(Error);
	});
}

void USequenceCheckout::GetCheckoutOptionsByERC1155Contract(const int64 ChainID, const FString& WalletAddress,
	const UERC1155SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId,
	const int64 Amount, const TSuccessCallback<FGetCheckoutOptionsResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	TMap<FString, int64> Amounts;
	Amounts.Add(TokenId, Amount);
	
	GetCheckoutOptionsByTokenIdAmounts(ChainID, WalletAddress, SaleContract, CollectionAddress, Amounts, OnSuccess, OnFailure);
}

void USequenceCheckout::GetCheckoutOptionsByERC721Contract(const int64 ChainID, const FString& WalletAddress,
	const UERC721SaleContract* SaleContract, const FString& CollectionAddress, const FString& TokenId,
	const int64 Amount, TSuccessCallback<FGetCheckoutOptionsResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString Endpoint = "CheckoutOptionsSalesContract";
	const FString Args = BuildArgs<FGetPrimarySaleCheckoutOptionsArgs>(FGetPrimarySaleCheckoutOptionsArgs { WalletAddress, SaleContract->ContractAddress, CollectionAddress, TokenId, Amount });

	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGetCheckoutOptionsResponse Response = BuildResponse<FGetCheckoutOptionsResponse>(Content);
		OnSuccess(Response);
	},
	OnFailure);
}


void USequenceCheckout::GenerateBuyTransaction(const int64 ChainID, const FString& WalletAddress, const FSeqOrder& Order, const int64 Amount, const FAdditionalFee& AdditionalFee, const EWalletKind WalletKind,
                                               TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	TArray<FOrderData> OrdersData;
	OrdersData.Reserve(1);
	OrdersData.Add(FOrderData(Order.OrderId, FString::Printf(TEXT("%lld"), Amount)));

	TArray<FAdditionalFee> AdditionalFees;
	if (AdditionalFee.IsEmpty() == false)
	{
		AdditionalFees.Reserve(1);
		AdditionalFees.Add(AdditionalFee);
	}

	const FString Endpoint = "GenerateBuyTransaction";
	const FString Args = BuildArgs<FGenerateBuyTransaction>(FGenerateBuyTransaction{ Order.CollectionContractAddress, WalletAddress, Order.Marketplace, OrdersData, AdditionalFees, WalletKind });
	
	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
		OnSuccess(Response);
	}, 
	OnFailure);
}

void USequenceCheckout::GenerateSellTransaction(const int64 ChainID, const FString& WalletAddress, const FSeqOrder& Order, const int64 Amount, const FAdditionalFee& AdditionalFee, const EWalletKind WalletKind,
	TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	TArray<FOrderData> OrdersData;
	OrdersData.Reserve(1);
	OrdersData.Add(FOrderData(Order.OrderId, FString::Printf(TEXT("%lld"), Amount)));

	TArray<FAdditionalFee> AdditionalFees;
	if (AdditionalFee.IsEmpty() == false)
	{
		AdditionalFees.Reserve(1);
		AdditionalFees.Add(AdditionalFee);
	}

	const FString Endpoint = "GenerateSellTransaction";
	const FString Args = BuildArgs<FGenerateSellTransaction>(FGenerateSellTransaction{ Order.CollectionContractAddress, WalletAddress, Order.Marketplace, OrdersData, AdditionalFees, WalletKind });;

	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
		OnSuccess(Response);
	},
	OnFailure);
}

void USequenceCheckout::GenerateListingTransaction(const int64 ChainID, const FString& WalletAddress, const FString& CollectionAddress, const FString& TokenId, const int64 Amount, const EContractType ContractType,
	const FString& CurrencyTokenAddress, const int64 PricePerToken, const FDateTime Expiry, const EOrderbookKind OrderbookKind, const EWalletKind WalletKind, 
	TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	const int64 EpochTime = Expiry.ToUnixTimestamp();
	const FString Endpoint = "GenerateListingTransaction";
	const FString Args = BuildArgs<FGenerateListingTransactionArgs>(FGenerateListingTransactionArgs {
		CollectionAddress, WalletAddress, ContractType, OrderbookKind, FCreateReq {
		TokenId, FString::Printf(TEXT("%lld"), Amount), FString::Printf(TEXT("%lld"), EpochTime), CurrencyTokenAddress, FString::Printf(TEXT("%lld"), PricePerToken)
		},  WalletKind
	});

	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
		OnSuccess(Response);
	},
	OnFailure);
}

void USequenceCheckout::GenerateOfferTransaction(const int64 ChainID, const FString& WalletAddress, const FString& CollectionAddress, const FString& TokenId, const int64 Amount,
	const EContractType ContractType, const FString& CurrencyTokenAddress, const int64 PricePerToken, const FDateTime Expiry,
	const EOrderbookKind OrderbookKind, const EWalletKind WalletKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	const int64 EpochTime = Expiry.ToUnixTimestamp();
	const FString Endpoint = "GenerateOfferTransaction";
	const FString Args = BuildArgs<FGenerateOfferTransactionArgs>(FGenerateOfferTransactionArgs {
		CollectionAddress, WalletAddress, ContractType, OrderbookKind, FCreateReq {
		TokenId, FString::Printf(TEXT("%lld"), Amount), FString::Printf(TEXT("%lld"), EpochTime), CurrencyTokenAddress, FString::Printf(TEXT("%lld"), PricePerToken)
		},  WalletKind
	});

	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
		OnSuccess(Response);
	},
	OnFailure);
}

void USequenceCheckout::GenerateCancelTransaction(const int64 ChainID, const FString& WalletAddress, const FString& CollectionAddress, const FString& OrderId,
	const EMarketplaceKind MarketplaceKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString Endpoint = "GenerateCancelTransaction";
	const FString Args = BuildArgs<FGenerateCancelTransactionRequest>(FGenerateCancelTransactionRequest {
		CollectionAddress, WalletAddress, MarketplaceKind, OrderId
	});

	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
		OnSuccess(Response);
	},
	OnFailure);
}

void USequenceCheckout::GenerateCancelTransactionByOrder(const int64 ChainID, const FString& WalletAddress, const FString& CollectionAddress, const FSeqOrder& Order,
	const EMarketplaceKind MarketplaceKind, TSuccessCallback<FGenerateTransactionResponse> OnSuccess, const FFailureCallback& OnFailure)
{
	GenerateCancelTransaction(ChainID, WalletAddress, CollectionAddress, Order.OrderId, MarketplaceKind, OnSuccess, OnFailure);
}
