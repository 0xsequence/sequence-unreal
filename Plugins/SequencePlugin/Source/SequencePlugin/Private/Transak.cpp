// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Transak.h"
#include "Http.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "HttpManager.h"
#include "Util/Log.h"
#include "Util/SequenceSupport.h"

UTransakOnRamp::UTransakOnRamp(){}

UTransakOnRamp * UTransakOnRamp::Init(const FString& WalletAddressIn)
{
	UTransakOnRamp * Transak = NewObject<UTransakOnRamp>();
	Transak->WalletAddress = WalletAddressIn;
	return Transak;
}

void UTransakOnRamp::GetSupportedCountries(TSuccessCallback<TArray<FSupportedCountry>> OnSuccess, FFailureCallback OnFailure)
{
	const FString& Url = "https://api.transak.com/api/v2/countries";
	SEQ_LOG(Log, TEXT("%s"), *Url);
	
	const TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("GET");
	http_post_req->SetTimeout(15);
	http_post_req->SetURL(Url);
	http_post_req->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
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
	http_post_req->ProcessRequest();
}

FString UTransakOnRamp::GetTransakLink(const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn)
{
	const FAddFundsSettings AddFundsSettings(WalletAddress,FiatCurrencyIn,DefaultFiatAmountIn,DefaultCryptoCurrencyIn,NetworksIn);
	const FOnOffQueryParameters QueryParameters(WalletAddress,AddFundsSettings,DisableWalletAddressFormIn);
	return ("https://global.transak.com?" + QueryParameters.AsQueryParameters()).Replace(TEXT(" "), TEXT(""));
}

void UTransakOnRamp::OpenTransakLink(const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn)
{
	FString * ErrorPtr = nullptr;
	FPlatformProcess::LaunchURL(*GetTransakLink(FiatCurrencyIn,DefaultFiatAmountIn,DefaultCryptoCurrencyIn,NetworksIn,DisableWalletAddressFormIn),TEXT(""),ErrorPtr);
	if (ErrorPtr)
	{
		SEQ_LOG(Error,TEXT("Browser LaunchError: %s"), **ErrorPtr);
	}
}