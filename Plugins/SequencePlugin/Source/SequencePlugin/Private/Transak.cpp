// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "Transak.h"
#include "Http.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "HttpManager.h"
#include "Indexer/IndexerSupport.h"

UTransakOnRamp::UTransakOnRamp(){}

UTransakOnRamp * UTransakOnRamp::Init(const FString& WalletAddressIn)
{
	UTransakOnRamp * Transak = NewObject<UTransakOnRamp>();
	Transak->WalletAddress = WalletAddressIn;
	return Transak;
}

void UTransakOnRamp::GetSupportedCountries(TSuccessCallback<TArray<FSupportedCountry>> OnSuccess, FFailureCallback OnFailure)
{
	const TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("GET");
	http_post_req->SetHeader("","");
	http_post_req->SetTimeout(30);
	http_post_req->SetURL("https://api.transak.com/api/v2/countries");
	http_post_req->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		if(bWasSuccessful)
		{
			const FString Content = Request->GetResponse()->GetContentAsString();
			const FSupportedCountryResponse ParsedResponse = UIndexerSupport::JSONStringToStruct<FSupportedCountryResponse>(Content);
			OnSuccess(ParsedResponse.Response);
		}
		else
		{
			if(Request.IsValid() && Request->GetResponse().IsValid())
			{
				OnFailure(FSequenceError(RequestFail, "Request failed: " + Request->GetResponse()->GetContentAsString()));
			}
			else
			{
				OnFailure(FSequenceError(RequestFail, "Request failed: Invalid Request Pointer"));
			}
		}
	});
	http_post_req->ProcessRequest();
}

FString UTransakOnRamp::GetTransakLink(const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn)
{
	const FAddFundsSettings AddFundsSettings(WalletAddress,FiatCurrencyIn,DefaultFiatAmountIn,DefaultCryptoCurrencyIn,NetworksIn);
	const FOnOffQueryParameters QueryParameters(WalletAddress,AddFundsSettings,DisableWalletAddressFormIn);
	return "https://global.transak.com?" + QueryParameters.AsQueryParameters();
}

void UTransakOnRamp::OpenTransakLink(const FString& FiatCurrencyIn, const FString& DefaultFiatAmountIn, const FString& DefaultCryptoCurrencyIn, const FString& NetworksIn, bool DisableWalletAddressFormIn)
{
	FString * ErrorPtr = nullptr;
	FPlatformProcess::LaunchURL(*GetTransakLink(FiatCurrencyIn,DefaultFiatAmountIn,DefaultCryptoCurrencyIn,NetworksIn,DisableWalletAddressFormIn),TEXT(""),ErrorPtr);
	if (ErrorPtr)
	{
		UE_LOG(LogTemp,Error,TEXT("Browser LaunchError: %s"), **ErrorPtr);
	}
}