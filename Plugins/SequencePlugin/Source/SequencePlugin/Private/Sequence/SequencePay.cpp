#include "Sequence/SequencePay.h"
#include "HttpModule.h"
#include "Http.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"
#include "ConfigFetcher.h"
#include "Indexer/SequenceIndexer.h"
#include "Marketplace/Structs/SeqGetLifiChainsResponse.h"
#include "Marketplace/Structs/SeqGetLifiSwapRoutesResponse.h"
#include "Marketplace/Structs/SeqGetSwapPricesArgs.h"
#include "Marketplace/Structs/SeqGetSwapQuoteArgs.h"
#include "Marketplace/Structs/SeqGetSwapQuoteResponse.h"
#include "Marketplace/Structs/SeqLifiSwapRoute.h"
#include "Marketplace/Structs/SeqGetLifiTokensArgs.h"
#include "Marketplace/Structs/SeqSwapPrice.h"
#include "Marketplace/Structs/SeqSwapQuote.h"

void USequencePay::GetSupportedOnRampCountries(TSuccessCallback<TArray<FSupportedCountry>> OnSuccess, FFailureCallback OnFailure)
{
	const FString& Url = "https://api.transak.com/api/v2/countries";
	SEQ_LOG(Log, TEXT("%s"), *Url);
	
	const TSharedRef<IHttpRequest> GetRequest = FHttpModule::Get().CreateRequest();
	GetRequest->SetVerb("GET");
	GetRequest->SetTimeout(15);
	GetRequest->SetURL(Url);
	GetRequest->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if(bWasSuccessful)
		{
			const FString Content = Request->GetResponse()->GetContentAsString();
			const FSupportedCountryResponse ParsedResponse = USequenceSupport::JSONStringToStruct<FSupportedCountryResponse>(Content);
			OnSuccess(ParsedResponse.Response);
			SEQ_LOG(Log, TEXT("%s"), *Content);
		}
		else
		{
			if(Request.IsValid() && Request->GetResponse().IsValid())
			{
				const FString& Content = Request->GetResponse()->GetContentAsString();
				OnFailure(FSequenceError(RequestFail, "Request failed: " + Content));
				SEQ_LOG_EDITOR(Log, TEXT("%s"), *Content);
			}
			else
			{
				const FString& Error = "Request failed: Invalid Request Pointer";
				OnFailure(FSequenceError(RequestFail, Error));
				SEQ_LOG(Error, TEXT("%s"), *Error);
			}
		}
	});
	GetRequest->ProcessRequest();
}

FString USequencePay::GetOnRampLink(const FString& WalletAddress, const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn)
{
	const FAddFundsSettings AddFundsSettings(WalletAddress,FiatCurrencyIn,DefaultFiatAmountIn,DefaultCryptoCurrencyIn,NetworksIn);
	const FOnOffQueryParameters QueryParameters(WalletAddress,AddFundsSettings,DisableWalletAddressFormIn);
	return ("https://global.transak.com?" + QueryParameters.AsQueryParameters()).Replace(TEXT(" "), TEXT(""));
}

void USequencePay::OpenOnRampInExternalBrowser(const FString& WalletAddress, const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn)
{
	FString * ErrorPtr = nullptr;
	FPlatformProcess::LaunchURL(*this->GetOnRampLink(WalletAddress,FiatCurrencyIn,DefaultFiatAmountIn,DefaultCryptoCurrencyIn,NetworksIn,DisableWalletAddressFormIn),TEXT(""),ErrorPtr);
	if (ErrorPtr)
	{
		SEQ_LOG(Error,TEXT("Browser LaunchError: %s"), **ErrorPtr);
	}
}

void USequencePay::GetSupportedSwapChains(const TSuccessCallback<FSeqGetLifiChainsResponse>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString EndPoint = "GetLifiChains";
	HTTPPostSwapAPI(EndPoint, "", [this, OnSuccess](const FString& Content)
	{
		const FSeqGetLifiChainsResponse Chains = USequenceSupport::JSONStringToStruct<FSeqGetLifiChainsResponse>(Content);
		OnSuccess(Chains);
	}, OnFailure);
}

void USequencePay::GetSupportedSwapTokens(const TArray<int64>& ChainIds, const TSuccessCallback<FSeqGetLifiTokensResponse>& OnSuccess, const FFailureCallback& OnFailure)
{
	const FSeqGetLifiTokensArgs Args {
		ChainIds
	};
	
	const FString EndPoint = "GetLifiTokens";
	HTTPPostSwapAPI(EndPoint, BuildArgs(Args), [this, OnSuccess](const FString& Content)
	{
		const FSeqGetLifiTokensResponse Response = USequenceSupport::JSONStringToStruct<FSeqGetLifiTokensResponse>(Content);
		OnSuccess(Response);
	}, OnFailure);
}

void USequencePay::GetSwapPrice(const int64 ChainID, const FString& WalletAddress, const FString& SellCurrency, const FString& BuyCurrency, const FString& BuyAmount, const TSuccessCallback<FSeqSwapPrice>& OnSuccess, const FFailureCallback& OnFailure)
{
	this->GetSwapPrices(ChainID, WalletAddress, BuyCurrency, BuyAmount, [SellCurrency, OnSuccess, OnFailure](TArray<FSeqSwapPrice> Prices)
	{
		for (const FSeqSwapPrice Price : Prices)
		{
			if (Price.CurrencyAddress == SellCurrency)
			{
				OnSuccess(Price);
				return;
			}
		}

		OnFailure(FSequenceError { ResponseParseError, "The given sell currency is not available as a swap option for this user." });
	}, OnFailure);
}

void USequencePay::GetSwapPrices(const int64 ChainID, const FString& WalletAddress, const FString& BuyCurrency,
	const FString& BuyAmount, const TSuccessCallback<TArray<FSeqSwapPrice>>& OnSuccess,
	const FFailureCallback& OnFailure)
{
	const FString EndPoint = "GetLifiSwapRoutes";
	FGetSwapPricesArgs Args {
		ChainID,
		BuyCurrency,
		BuyAmount,
		WalletAddress
	};
	
	HTTPPostSwapAPI(EndPoint, BuildArgs(Args), [this, ChainID, BuyCurrency, OnSuccess](const FString& Content)
	{
		const FSeqGetLifiSwapRoutesResponse Response = USequenceSupport::JSONStringToStruct<FSeqGetLifiSwapRoutesResponse>(Content);
		const TArray<FSeqLifiSwapRoute> SwapRoutes = Response.Routes;

		TArray<FSeqSwapPrice> SwapPrices;
		for (const FSeqLifiSwapRoute& Route : SwapRoutes)
		{
			if (Route.FromChainId != ChainID ||
				Route.ToChainId != ChainID ||
				!Route.ContainsToken(BuyCurrency))
			{
				continue;
			}

			for (const FSeqSwapToken& FromToken : Route.FromTokens)
			{
				SwapPrices.Add(FSeqSwapPrice(FromToken.Address, FromToken.Price));
			}
		}
		
		OnSuccess(SwapPrices);
	}, OnFailure);
}

void USequencePay::GetSwapQuote(const int64 ChainID, const FString& WalletAddress, const FString& BuyCurrency,
	const FString& SellCurrency, const FString& BuyAmount, const FString& SellAmount, const bool IncludeApprove,
	const TSuccessCallback<FSeqSwapQuote>& OnSuccess, const FFailureCallback& OnFailure)
{
	if (!BuyAmount.IsEmpty() && !SellAmount.IsEmpty())
	{
		OnFailure(FSequenceError {InvalidArgument, "Please only define either BuyAmount OR SellAmount."});
		return;
	}

	const FSeqGetSwapQuoteParams Params {
		ChainID,
		WalletAddress,
		BuyCurrency,
		SellCurrency,
		BuyAmount,
		SellAmount,
		IncludeApprove
	};
		
	const FSeqGetSwapQuoteArgs Args {
		Params
	};

	const FString EndPoint = "GetLifiSwapQuote";
	HTTPPostSwapAPI(EndPoint, BuildArgs(Args), [this, OnSuccess](const FString& Content)
	{
		const FGetSwapQuoteResponse Response = USequenceSupport::JSONStringToStruct<FGetSwapQuoteResponse>(Content);
		OnSuccess(Response.Quote);
	}, OnFailure);
}

template < typename T> FString USequencePay::BuildArgs(T StructIn)
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

void USequencePay::HTTPPostSwapAPI(const FString& Endpoint, const FString& Args,
	const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString RequestURL = "https://api.sequence.app/rpc/API/" + Endpoint;
	const TSharedRef<IHttpRequest> HTTP_Post_Req = FHttpModule::Get().CreateRequest();

	FString AccessKey = UConfigFetcher::GetConfigVar("ProjectAccessKey");
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
	 
	SEQ_LOG(Display, TEXT("Requesting %s with payload %s"), *RequestURL, *Args);  
	
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

	HTTP_Post_Req->ProcessRequest();
}
