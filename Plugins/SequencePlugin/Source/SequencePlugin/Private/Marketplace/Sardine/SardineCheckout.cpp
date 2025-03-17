// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.


#include "Marketplace/Sardine/SardineCheckout.h"

#include "ConfigFetcher.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Marketplace/Marketplace.h"
#include "Marketplace/Sardine/Structs/SardineEnabledTokensResponse.h"
#include "Marketplace/Sardine/Structs/SardineGetQuoteArgs.h"
#include "Marketplace/Sardine/Structs/SardineGetQuoteResponse.h"
#include "Marketplace/Sardine/Structs/SardineSupportedFiatCurrenciesResponse.h"
#include "Marketplace/Sardine/Structs/SardineSupportedRegionsResponse.h"
#include "Marketplace/Sardine/Structs/SardineSupportedTokensResponse.h"
#include "Util/Log.h"

FString USardineCheckout::Url(const FString& EndPoint) const
{
	return _baseUrl + "/" + EndPoint;
}

void USardineCheckout::SardineGetNFTCHeckoutToken(FSardineGetNFTCheckoutTokenArgs Args,
	TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure)
{
	if(Args.TokenAddress == FAddress::New().ToHex())
	{
		OnFailure(FSequenceError(InvalidArgument, "Sardine doesn't support native currency checkout; please choose a different payment token"));
		return;
	}

	HTTPPost("SardineGetNFTCheckoutToken", BuildArgs<FSardineGetNFTCheckoutTokenArgs>(Args), [this, OnSuccess](const FString& Content)
		{
			const FSardineNFTCheckout Response = this->BuildResponse<FSardineNFTCheckout>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

void USardineCheckout::HTTPPost(const FString& Endpoint, const FString& Args,
                                const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString RequestURL = this->Url(Endpoint);

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

void USardineCheckout::CheckSardineWhiteListStatus(FString Address, TSuccessCallback<bool> OnSuccess,
	const FFailureCallback& OnFailure)
{
	
}

void USardineCheckout::SardineGetQuote(TSuccessCallback<FSardineQuote> OnSuccess, const FFailureCallback& OnFailure, FString WalletAddress, FSardineToken Token, u_long Amount, ESardinePaymentType PaymentType,
                                       TOptional<FSardineFiatCurrency> QuotedCurrency, ESardineQuoteType QuoteType)
{
	const FString Endpoint = "SardineGetQuote";
	
	FSardineGetQuoteArgs Args {
		Token.AssetSymbol,
		Token.Network,
		Amount,
		QuotedCurrency.IsSet() ? QuotedCurrency->CurrencySymbol : "",
		PaymentType,
		QuoteType,
		WalletAddress
	};

	HTTPPost(Endpoint, BuildArgs<FSardineGetQuoteArgs>(Args), [this, OnSuccess](const FString& Content)
		{
			const FSardineGetQuoteResponse Response = this->BuildResponse<FSardineGetQuoteResponse>(Content);
			OnSuccess(Response.Quote);
		}, OnFailure);
}

void USardineCheckout::SardineGetClientToken(TSuccessCallback<FString> OnSuccess, const FFailureCallback& OnFailure)
{
	
}

void USardineCheckout::SardineGetNFTCheckoutToken(TArray<FSeqCollectibleOrder> Orders, long Quantity,
	FString RecipientAddress, TArray<FAdditionalFee> AdditionalFee, FString MarketPlaceContractAddress,
	TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure)
{
	if(Orders.Num() < 1)
	{
		OnFailure(FSequenceError(InvalidArgument, "Orders must contain at least one collectible"));
		return;
	}

	if(Orders[0].Order.PriceCurrencyAddress == FAddress::New().ToHex())
	{
		OnFailure(FSequenceError(InvalidArgument, "Sardine doesn't support native currency checkout; please choose a different payment token"));
		return;
	}

	//TODO Generate steps from checkout
}

void USardineCheckout::SardineGetNFTCheckoutToken(int64 ChainId, UERC1155SaleContract SaleContract, FString CollectionAddress,
	long TokenID, long Amount, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure,
	FString RecipientAddress, TArray<uint8> data, TArray<uint8> Proof)
{
	if(Amount < 1)
	{
		OnFailure(FSequenceError(InvalidArgument, "Amount must be greater than 0"));
		return;
	}

	UMarketplace* Marketplace = NewObject<UMarketplace>();

	Marketplace->GetCollectible(ChainId, CollectionAddress, FString::FromInt(TokenID), [this, &SaleContract, Amount, OnSuccess, RecipientAddress, data, Proof](FSeqTokenMetaData MetaData)
		{
			//TODO
		}, [OnFailure](FSequenceError Error)
		{
			SEQ_LOG_EDITOR(Error, TEXT("Failed to get collectible: %s"), *Error.Message);
			OnFailure(Error);
		});
	
	//FSeqTokenMetaData metaData = 
}

void USardineCheckout::SardineGetSupportedRegions(TSuccessCallback<TArray<FSardineRegion>> OnSuccess,
	const FFailureCallback& OnFailure)
{
	const FString Endpoint = "SardineGetSupportedRegions";
	HTTPPost(Endpoint, "", [this, OnSuccess](const FString& Content)
		{
			const FSardineSupportedRegionsResponse Response = this->BuildResponse<FSardineSupportedRegionsResponse>(Content);
			OnSuccess(Response.Regions);
		}, OnFailure);
}

void USardineCheckout::SardineGetSupportedFiatCurrencies(TSuccessCallback<TArray<FSardineFiatCurrency>> OnSuccess,
                                                         const FFailureCallback& OnFailure)
{
	const FString Endpoint = "SardineGetSupportedFiatCurrencies";
	HTTPPost(Endpoint, "", [this, OnSuccess](const FString& Content)
		{
			const FSardineSupportedFiatCurrenciesResponse Response = this->BuildResponse<FSardineSupportedFiatCurrenciesResponse>(Content);
			OnSuccess(Response.Tokens);
		}, OnFailure);
}

void USardineCheckout::SardineGetSupportedTokens(TSuccessCallback<TArray<FSardineSupportedToken>> OnSuccess,
                                                 const FFailureCallback& OnFailure)
{
	const FString Endpoint = "SardineGetSupportedTokens";
	HTTPPost(Endpoint, "", [this, OnSuccess](const FString& Content)
		{
			const FSardineSupportedTokensResponse Response = this->BuildResponse<FSardineSupportedTokensResponse>(Content);
			OnSuccess(Response.Tokens);
		}, OnFailure);
}

void USardineCheckout::SardineGetEnabledTokens(TSuccessCallback<TArray<FSardineEnabledToken>> OnSuccess,
                                               const FFailureCallback& OnFailure)
{
	const FString Endpoint = "SardineGetEnabledTokens";
	HTTPPost(Endpoint, "", [this, OnSuccess](const FString& Content)
		{
			const FSardineEnabledTokensResponse Response = this->BuildResponse<FSardineEnabledTokensResponse>(Content);
			OnSuccess(Response.Tokens);
		}, OnFailure);
}
