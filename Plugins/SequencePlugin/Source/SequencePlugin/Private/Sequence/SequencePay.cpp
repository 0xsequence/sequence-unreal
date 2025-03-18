// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Sequence/SequencePay.h"
#include "HttpModule.h"
#include "Http.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"

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
