// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/SequencePay.h"
#include "HttpModule.h"
#include "Http.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"
#include "ConfigFetcher.h"
#include "Indexer/SequenceIndexer.h"
#include "Marketplace/Structs/SeqGetSwapPriceArgs.h"
#include "Marketplace/Structs/SeqGetSwapPricesArgs.h"
#include "Marketplace/Structs/SeqGetSwapPricesResponse.h"
#include "Marketplace/Structs/SeqGetSwapQuoteArgs.h"
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

void USequencePay::GetSwapPrice(const int64 ChainID, const FString& SellCurrency, const FString& BuyCurrency,
	const FString& BuyAmount, const TSuccessCallback<FSeqSwapPrice>& OnSuccess, const FFailureCallback& OnFailure,
	const int SlippagePercentage)
{
	const FString EndPoint = "GetSwapPermit2Price";
	FGetSwapPriceArgs Args {
		BuyCurrency,
		SellCurrency,
		BuyAmount,
		ChainID,
		SlippagePercentage		
	};
	HTTPPostSwapAPI(EndPoint, BuildArgs(Args), [this, OnSuccess, OnFailure](const FString& Content)
		{
			FSeqSwapPrice Response;
			
			if(!Response.FromResponse(Content))
			{
				OnFailure(FSequenceError { ResponseParseError, "Failed to parse response" });
				return;
			}
			
			OnSuccess(Response);
		}, OnFailure);
}

void USequencePay::GetSwapPrices(const int64 ChainID, const FString& UserWallet, const FString& BuyCurrency,
	const FString& BuyAmount, const TSuccessCallback<TArray<FSeqSwapPrice>>& OnSuccess,
	const FFailureCallback& OnFailure, const int SlippagePercentage)
{
	const FString EndPoint = "GetSwapPermit2Prices";
	FGetSwapPricesArgs Args {
		UserWallet,
		BuyCurrency,
		BuyAmount,
		static_cast<int>(ChainID),
		SlippagePercentage
	};
	HTTPPostSwapAPI(EndPoint, BuildArgs(Args), [this, OnSuccess, OnFailure](const FString& Content)
		{
			TArray<FSeqSwapPrice> Response;
			TSharedPtr<FJsonObject> Json;

			if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Content), Json) || !Json->HasField(TEXT("swapPermit2Prices")))
			{
				OnFailure(FSequenceError { ResponseParseError, "Failed to parse response" });
				return;
			}

			TArray<TSharedPtr<FJsonValue>> Array = Json->GetArrayField(TEXT("swapPermit2Prices"));

			for (TSharedPtr<FJsonValue> Value : Array)
			{
				FSeqSwapPrice SwapPrice;
				if (!SwapPrice.FromJson(Value->AsObject()))
				{
					OnFailure(FSequenceError { ResponseParseError, "Failed to parse response" });
					return;
				}
				Response.Add(SwapPrice);
			}
		
			OnSuccess(Response);
		}, OnFailure);
}

void USequencePay::GetSwapQuote(const int64 ChainID, const FString& UserWallet, const FString& BuyCurrency,
	const FString& SellCurrency, const FString& BuyAmount, const bool IncludeApprove,
	const TSuccessCallback<FSeqSwapQuote>& OnSuccess, const FFailureCallback& OnFailure, const int SlippagePercentage)
{
	const FString EndPoint = "GetSwapQuoteV2";

	AssertWeHaveSufficientBalance(ChainID, UserWallet, BuyCurrency, SellCurrency, BuyAmount, [this, OnFailure, EndPoint, OnSuccess, ChainID, UserWallet, BuyCurrency, SellCurrency, BuyAmount, IncludeApprove, SlippagePercentage](void)
	{
		FGetSwapQuoteArgs Args {
			UserWallet,
			BuyCurrency,
			SellCurrency,
			BuyAmount,
			static_cast<int>(ChainID),
			IncludeApprove,
			SlippagePercentage
		};
		HTTPPostSwapAPI(EndPoint, BuildArgs(Args), [this, OnSuccess, OnFailure](const FString& Content)
			{
				FSeqSwapQuote Response;
			
				if(!Response.FromResponse(Content))
				{
					OnFailure(FSequenceError { ResponseParseError, "Failed to parse response" });
					return;
				}
				
				OnSuccess(Response);
			}, OnFailure);
	}, OnFailure, SlippagePercentage);
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
	 
	SEQ_LOG(Display, TEXT("body: %s"), *Args);  
	SEQ_LOG(Display, TEXT("request: %s"), *RequestURL);  
	
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

void USequencePay::AssertWeHaveSufficientBalance(const int64 ChainID, const FString& UserWallet, const FString& BuyCurrency,
	const FString& SellCurrency, const FString& BuyAmount, const TFunction<void ()>& OnSuccess,
	const FFailureCallback& OnFailure, const int SlippagePercentage)
{
	GetSwapPrice(ChainID, SellCurrency, BuyCurrency, BuyAmount, [this, OnFailure, ChainID, UserWallet, BuyCurrency, SellCurrency, BuyAmount, OnSuccess, SlippagePercentage](const FSeqSwapPrice& SwapPrice)
	{
		long Required = FCString::Atoi64(*SwapPrice.MaxPrice);
		
		USequenceIndexer* Indexer = NewObject<USequenceIndexer>();
		FSeqGetTokenBalancesArgs Args;
		Args.accountAddress = UserWallet;
		Args.contractAddress = SellCurrency;

		Indexer->GetTokenBalances(ChainID, Args, [this, Required, OnFailure, ChainID, UserWallet, BuyCurrency, SellCurrency, BuyAmount, SwapPrice, OnSuccess, SlippagePercentage](const FSeqGetTokenBalancesReturn& TokenBalances)
			{
				TArray<FSeqTokenBalance> SellCurrencies = TokenBalances.balances;
				long Have = 0;
				
				if(SellCurrencies.Num() > 0)
				{
					Have = SellCurrencies[0].balance;
				}
				
				SEQ_LOG(Display, TEXT("Have: %ld, Required: %ld"), Have, Required);

				if(Have < Required)
				{
					const FString ErrorString = FString::Format(TEXT("Insufficient balance of {0} to buy {1} of {2}, have {3}, need {4}"), 
																{ *SellCurrency, *BuyAmount, *BuyCurrency, FString::Printf(TEXT("%ld"), Have), FString::Printf(TEXT("%ld"), Required) });
					OnFailure(FSequenceError(InsufficientBalance, ErrorString));
				}
				else
				{
					OnSuccess();
				}
			}, OnFailure);
	}, OnFailure, SlippagePercentage);
}
