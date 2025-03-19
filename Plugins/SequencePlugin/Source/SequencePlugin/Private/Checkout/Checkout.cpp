#include "Checkout.h"

#include "Checkout/Structs/GenerateBuyTransaction.h"
#include "Checkout/Structs/GenerateCancelTransactionRequest.h"
#include "Checkout/Structs/GenerateListingTransactionArgs.h"
#include "Checkout/Structs/GenerateOfferTransactionArgs.h"
#include "Checkout/Structs/GenerateSellTransaction.h"
#include "Checkout/Structs/GetCheckoutOptionsArgs.h"
#include "Checkout/Structs/GetCheckoutOptionsResponse.h"
#include "Checkout/Structs/OrderData.h"
#include "Util/Log.h"

FString UCheckout::Url(const int64& TargetChainID, const FString& EndPoint) const
{
	FString Out_URL = HostName(TargetChainID);
	Out_URL.Append(this->Path);

	if (EndPoint.Len() > 0)
	{
		Out_URL.Append(EndPoint);
	}
	return Out_URL;
}

FString UCheckout::HostName(int64 TargetChainID)
{
	FString Hostname = "https://";
	Hostname.Append("marketplace-api.sequence.app/");
	Hostname.Append(USequenceSupport::GetNetworkNameForUrl(TargetChainID));
	return Hostname;
}

void UCheckout::HTTPPost(const int64& TargetChainID, const FString& Endpoint, const FString& Args,
                         const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString RequestURL = this->Url(TargetChainID, Endpoint);

	const TSharedRef<IHttpRequest> HTTP_Post_Req = FHttpModule::Get().CreateRequest();

	FString AccessKey = UConfigFetcher::GetConfigVar("ProjectAccessKey");
	if (AccessKey.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("AccessKey is empty! Failed to set HTTP header."));
		return;  
	}

	HTTP_Post_Req->SetVerb("POST");
	HTTP_Post_Req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HTTP_Post_Req->SetHeader(TEXT("Accept"), TEXT("application/json"));
	

	HTTP_Post_Req->SetHeader(TEXT("X-Access-Key"), *AccessKey);	
	HTTP_Post_Req->SetTimeout(30);
	HTTP_Post_Req->SetURL(RequestURL);
	HTTP_Post_Req->SetContentAsString(Args);
	 
	FString CurlCommand = FString::Printf(
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
				UE_LOG(LogTemp, Display, TEXT("Response: %s"), *Content);  
				OnSuccess(Content);
			}
			else
			{
				if (Request.IsValid() && Response.IsValid())
				{
					const FString ErrorMessage = Response->GetContentAsString();
					UE_LOG(LogTemp, Error, TEXT("Request failed: %s"), *ErrorMessage);  
					OnFailure(FSequenceError(RequestFail, "Request failed: " + ErrorMessage));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Request failed: Invalid Request Pointer")); 
					OnFailure(FSequenceError(RequestFail, "Request failed: Invalid Request Pointer"));
				}
			}
		});

	// Process the request
	HTTP_Post_Req->ProcessRequest();
}

UCheckout::UCheckout()
{
	this->ChainID = 0;
}

UCheckout::UCheckout(const int64& InChainID)
{
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

void UCheckout::SetChainID(const int64& InChainID)
{
	this->ChainID = InChainID;
}

void UCheckout::GetCheckoutOptions(const FString& WalletAddress, const TArray<FCheckoutOptionsMarketplaceOrder>& Orders, const int64 AdditionalFeeBps,
                                   const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure) const
{
	const FString Endpoint = "CheckoutOptionsMarketplace";
	const FString Args = BuildArgs<FGetCheckoutOptionsArgs>(FGetCheckoutOptionsArgs { WalletAddress, Orders, AdditionalFeeBps });

	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGetCheckoutOptionsResponse Response = BuildResponse<FGetCheckoutOptionsResponse>(Content);
		if (OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [this, OnFailure](const FSequenceError& Error)
	{
		if (OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void UCheckout::GetCheckoutOptionsByOrders(const FString& WalletAddress, const TArray<FSeqOrder>& Orders, const int64 AdditionalFeeBps,
	const FOnGetCheckoutOptionsResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure) const
{
	TArray<FCheckoutOptionsMarketplaceOrder> Options;
	Options.Reserve(Orders.Num());
	for (int32 i = 0; i < Orders.Num(); i++)
	{
		Options[i] = FCheckoutOptionsMarketplaceOrder(Orders[i].CollectionContractAddress, Orders[i].OrderId, Orders[i].Marketplace);
	}

	GetCheckoutOptions(WalletAddress, Options, AdditionalFeeBps, OnSuccess, OnFailure);
}

void UCheckout::GenerateBuyTransaction(const FString& WalletAddress, const FSeqOrder& Order, const int64 Amount, const FAdditionalFee& AdditionalFee, const EWalletKind WalletKind,
                                       FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const
{
	TArray<FOrderData> OrdersData;
	OrdersData.Reserve(1);
	OrdersData.Add(FOrderData(Order.OrderId, FString::Printf(TEXT("%lld"), Amount)));

	const TArray AdditionalFees { AdditionalFee };

	const FString Endpoint = "GenerateBuyTransaction";
	const FString Args = BuildArgs<FGenerateBuyTransaction>(FGenerateBuyTransaction{ Order.CollectionContractAddress, WalletAddress, Order.Marketplace, OrdersData, AdditionalFees, WalletKind });
	
	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
		if (OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [this, OnFailure](const FSequenceError& Error)
	{
		if (OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void UCheckout::GenerateSellTransaction(const FString& WalletAddress, const FSeqOrder& Order, const int64 Amount, const FAdditionalFee& AdditionalFee, const EWalletKind WalletKind,
	FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const
{
	TArray<FOrderData> OrdersData;
	OrdersData.Reserve(1);
	OrdersData.Add(FOrderData(Order.OrderId, FString::Printf(TEXT("%lld"), Amount)));

	const TArray AdditionalFees { AdditionalFee };

	const FString Endpoint = "GenerateSellTransaction";
	const FString Args = BuildArgs<FGenerateSellTransaction>(FGenerateSellTransaction{ Order.CollectionContractAddress, WalletAddress, Order.Marketplace, OrdersData, AdditionalFees, WalletKind });;

	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
		if (OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [this, OnFailure](const FSequenceError& Error)
	{
		if (OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void UCheckout::GenerateListingTransaction(const FString& WalletAddress, const FString& CollectionAddress, const FString& TokenId, const int64 Amount, const EContractType ContractType,
	const FString& CurrencyTokenAddress, const int64 PricePerToken, const FDateTime Expiry, const EOrderbookKind OrderbookKind, const EWalletKind WalletKind, 
	FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const
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
		if (OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [this, OnFailure](const FSequenceError& Error)
	{
		if (OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void UCheckout::GenerateOfferTransaction(const FString& WalletAddress, const FString& CollectionAddress, const FString& TokenId, const int64 Amount,
	const EContractType ContractType, const FString& CurrencyTokenAddress, const int64 PricePerToken, const FDateTime Expiry,
	const EOrderbookKind OrderbookKind, const EWalletKind WalletKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const
{
	int64 EpochTime = Expiry.ToUnixTimestamp();
	const FString Endpoint = "GenerateOfferTransaction";
	const FString Args = BuildArgs<FGenerateOfferTransactionArgs>(FGenerateOfferTransactionArgs {
		CollectionAddress, WalletAddress, ContractType, OrderbookKind, FCreateReq {
		TokenId, FString::Printf(TEXT("%lld"), Amount), FString::Printf(TEXT("%lld"), EpochTime), CurrencyTokenAddress, FString::Printf(TEXT("%lld"), PricePerToken)
		},  WalletKind
	});

	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
		if (OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [this, OnFailure](const FSequenceError& Error)
	{
		if (OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void UCheckout::GenerateCancelTransaction(const FString& WalletAddress, const FString& CollectionAddress, const FString& OrderId,
	const EMarketplaceKind MarketplaceKind, FOnGenerateTransactionResponseSuccess OnSuccess, FOnCheckoutFailure OnFailure) const
{
	const FString Endpoint = "GenerateCancelTransaction";
	const FString Args = BuildArgs<FGenerateCancelTransactionRequest>(FGenerateCancelTransactionRequest {
		CollectionAddress, WalletAddress, MarketplaceKind, OrderId
	});

	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
	{
		const FGenerateTransactionResponse Response = BuildResponse<FGenerateTransactionResponse>(Content);
		if (OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [this, OnFailure](const FSequenceError& Error)
	{
		if (OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void UCheckout::GenerateCancelTransactionByOrder(const FString& WalletAddress, const FString& CollectionAddress, const FSeqOrder& Order,
	const EMarketplaceKind MarketplaceKind, const FOnGenerateTransactionResponseSuccess OnSuccess, const FOnCheckoutFailure OnFailure) const
{
	GenerateCancelTransaction(WalletAddress, CollectionAddress, Order.OrderId, MarketplaceKind, OnSuccess, OnFailure);
}
