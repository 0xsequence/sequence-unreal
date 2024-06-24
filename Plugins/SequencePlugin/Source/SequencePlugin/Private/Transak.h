// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Transak.generated.h"

USTRUCT()
struct FPartner
{
	GENERATED_BODY()
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
	GENERATED_BODY()
	
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
	GENERATED_BODY()
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
	GENERATED_BODY()
public:
	FString WalletAddress = "";
	FString FiatCurrency = "";
	FString DefaultFiatAmount = "";
	FString DefaultCryptoCurrency = "USDC";
	FString Networks = "";
	const FString DefaultNetworks = "ethereum,mainnet,arbitrum,optimism,polygon,polygonzkevm,zksync,base,bnb,oasys,astar,avaxcchain";

	FAddFundsSettings(){}

	FAddFundsSettings(const FString& WalletAddressIn)
	{
		
	}
	
	
};

USTRUCT()
struct FOnOffQueryParameters
{
	GENERATED_BODY()
public:
	const FString ApiKey = "5911d9ec-46b5-48fa-a755-d59a715ff0cf"; // This can be hardcoded as it is a public API key
	FString ReferrerDomain = "";
	FString WalletAddress = "";
	FString FiatCurrency = "";
	bool DisableWalletAddressForm = false;
	FString DefaultFiatAmount = "";
	FString DefaultCryptoCurrency = "";
	FString Networks = "";

	FOnOffQueryParameters(){}

	FOnOffQueryParameters(const FString& WalletAddressIn)
	{
		
	}
};

USTRUCT()
struct FTransakOnRamp
{
	GENERATED_BODY()
private:
	FString WalletAddress = "";
public:
	FTransakOnRamp(){}

	FTransakOnRamp(const FString& WalletAddressIn)
	{
		WalletAddress = WalletAddressIn;
	}
	
};