// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "GeneralProjectSettings.h"
#include "Http.h"
#include "HttpManager.h"
#include "UObject/Object.h"
#include "Util/Async.h"
#include "Indexer/IndexerSupport.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Transak.generated.h"

USTRUCT()
struct FPartner
{
	GENERATED_USTRUCT_BODY()
public:
	FString Name = "";
	bool IsCardPayment = false;
	FString CurrencyCode = "";

	FPartner(){}

	FPartner(const FString& NameIn, bool IsCardPaymentIn, const FString& CurrencyCodeIn)
	{
		Name = NameIn;
		IsCardPayment = IsCardPaymentIn;
		CurrencyCode = CurrencyCodeIn;
	}
};

USTRUCT()
struct FSupportedCountry
{
	GENERATED_USTRUCT_BODY()
	
public:
	FString Alpha2 = "";
	FString Alpha3 = "";
	bool IsAllowed = false;
	bool IsLightKycAllowed = false;
	FString Name = "";
	TArray<FString> SupportedDocuments;
	FString CurrencyCode = "";
	TArray<FPartner> Partners;

	FSupportedCountry(){}

	FSupportedCountry(const FString& Alpha2In, const FString& Alpha3In, bool IsAllowedIn, bool IsLightKycAllowedIn,
					  const FString& NameIn, const TArray<FString>& SupportedDocumentsIn, const FString& CurrencyCodeIn,
					  const TArray<FPartner>& PartnersIn)
	{
		Alpha2 = Alpha2In;
		Alpha3 = Alpha3In;
		IsAllowed = IsAllowedIn;
		IsLightKycAllowed = IsLightKycAllowedIn;
		Name = NameIn;
		SupportedDocuments = SupportedDocumentsIn;
		CurrencyCode = CurrencyCodeIn;
		Partners = PartnersIn;
	}
};

USTRUCT()
struct FSupportedCountryResponse
{
	GENERATED_USTRUCT_BODY()
	TArray<FSupportedCountry> Response;

	FSupportedCountryResponse(){}

	FSupportedCountryResponse(const TArray<FSupportedCountry>& ResponseIn)
	{
		Response = ResponseIn;
	}
};

USTRUCT()
struct FAddFundsSettings
{
	GENERATED_USTRUCT_BODY()
public:
	FString WalletAddress = "";
	FString FiatCurrency = "";
	FString DefaultFiatAmount = "";
	FString DefaultCryptoCurrency = "USDC";
	FString Networks = "";
	inline const static FString DefaultSetNetworks = "ethereum,mainnet,arbitrum,optimism,polygon,polygonzkevm,zksync,base,bnb,oasys,astar,avaxcchain";
	inline const static FString DefaultSetCryptoCurrency = "USDC";
	
	FAddFundsSettings(){}

	FAddFundsSettings(const FString& WalletAddressIn, const FString& FiatCurrencyIn = "USD", const FString& DefaultFiatAmountIn = "50", const FString& DefaultCryptoCurrencyIn = DefaultSetCryptoCurrency, const FString& NetworksIn = DefaultSetNetworks)
	{
		WalletAddress = WalletAddressIn;
		FiatCurrency = FiatCurrencyIn;
		DefaultFiatAmount = DefaultFiatAmountIn;
		DefaultCryptoCurrency = DefaultCryptoCurrencyIn;
		Networks = NetworksIn;
	}	
};

USTRUCT()
struct FOnOffQueryParameters
{
	GENERATED_USTRUCT_BODY()
public:
	const static inline FString ApiKey = "5911d9ec-46b5-48fa-a755-d59a715ff0cf"; // This can be hardcoded as it is a public API key
	FString ReferrerDomain = "";
	FString WalletAddress = "";
	FString FiatCurrency = "";
	bool DisableWalletAddressForm = false;
	FString DefaultFiatAmount = "";
	FString DefaultCryptoCurrency = "";
	FString Networks = "";

	FOnOffQueryParameters(){}

	FOnOffQueryParameters(const FString& WalletAddressIn, const FAddFundsSettings& AddFundSettingsIn, bool DisableWalletAddressFormIn = true)
	{
		WalletAddress = WalletAddressIn;
		const UGeneralProjectSettings& ProjectSettings = *GetDefault<UGeneralProjectSettings>(); 
		ReferrerDomain = "sequence-unreal: " + ProjectSettings.ProjectName;
		FiatCurrency = AddFundSettingsIn.FiatCurrency;
		DisableWalletAddressForm = DisableWalletAddressFormIn;
		DefaultFiatAmount = AddFundSettingsIn.DefaultFiatAmount;
		DefaultCryptoCurrency = AddFundSettingsIn.DefaultCryptoCurrency;
		Networks = AddFundSettingsIn.Networks;
	}
	
	FString AsQueryParameters() const
	{
		const FString DisableWalletFormString = (DisableWalletAddressForm) ? "true" : "false";
		const FString Ret = "apiKey=" + ApiKey +
			"&referrerDomain=" + ReferrerDomain +
			"&walletAddress=" + WalletAddress +
			"&fiatCurrency=" + FiatCurrency +
			"&disableWalletAddressForm=" + DisableWalletFormString +
			"&defaultFiatAmount=" + DefaultFiatAmount +
			"&defaultCryptoCurrency=" + DefaultCryptoCurrency +
			"&networks=" + Networks;
		return Ret;
	}
};

USTRUCT()
struct FTransakOnRamp
{
	GENERATED_USTRUCT_BODY()
private:
	FString WalletAddress = "";
public:
	FTransakOnRamp(){}

	FTransakOnRamp(const FString& WalletAddressIn)
	{
		WalletAddress = WalletAddressIn;
	}
	
	static void GetSupportedCountries(TSuccessCallback<TArray<FSupportedCountry>> OnSuccess, FFailureCallback OnFailure)
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
				UE_LOG(LogTemp,Display,TEXT("Response: %s"), *Content);
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

	FString GetTransakLink(const FString& FiatCurrencyIn = "USD", const FString& DefaultFiatAmountIn = "50", const FString& DefaultCryptoCurrencyIn = FAddFundsSettings::DefaultSetCryptoCurrency, const FString& NetworksIn = FAddFundsSettings::DefaultSetNetworks, bool DisableWalletAddressFormIn = true)
	{
		const FAddFundsSettings AddFundsSettings(WalletAddress,FiatCurrencyIn,DefaultFiatAmountIn,DefaultCryptoCurrencyIn,NetworksIn);
		const FOnOffQueryParameters QueryParameters(WalletAddress,AddFundsSettings,DisableWalletAddressFormIn);
		return "https://global.transak.com?" + QueryParameters.AsQueryParameters();
	}

	void OpenTransakLink(const FString& FiatCurrencyIn = "USD", const FString& DefaultFiatAmountIn = "50", const FString& DefaultCryptoCurrencyIn = FAddFundsSettings::DefaultSetCryptoCurrency, const FString& NetworksIn = FAddFundsSettings::DefaultSetNetworks, bool DisableWalletAddressFormIn = true)
	{
		FString * ErrorPtr = nullptr;
		FGenericPlatformProcess::LaunchURL(*GetTransakLink(FiatCurrencyIn,DefaultFiatAmountIn,DefaultCryptoCurrencyIn,NetworksIn,DisableWalletAddressFormIn),TEXT(""),ErrorPtr);
		if (ErrorPtr)
		{
			UE_LOG(LogTemp,Error,TEXT("Browser LaunchError: %s"), **ErrorPtr);
		}
	}
};