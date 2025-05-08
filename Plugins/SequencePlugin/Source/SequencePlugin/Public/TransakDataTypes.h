#pragma once
#include "CoreMinimal.h"
#include "GeneralProjectSettings.h"
#include "TransakDataTypes.generated.h"

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FPartner
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	bool IsCardPayment = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString CurrencyCode = "";

	FPartner(){}

	FPartner(const FString& NameIn, bool IsCardPaymentIn, const FString& CurrencyCodeIn)
	{
		Name = NameIn;
		IsCardPayment = IsCardPaymentIn;
		CurrencyCode = CurrencyCodeIn;
	}
};

USTRUCT(Blueprintable)
struct SEQUENCEPLUGIN_API FSupportedCountry
{
	GENERATED_USTRUCT_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Alpha2 = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Alpha3 = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	bool IsAllowed = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	bool IsLightKycAllowed = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	TArray<FString> SupportedDocuments;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
	FString CurrencyCode = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "0xSequence")
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
struct SEQUENCEPLUGIN_API FSupportedCountryResponse
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	TArray<FSupportedCountry> Response;

	FSupportedCountryResponse(){}

	FSupportedCountryResponse(const TArray<FSupportedCountry>& ResponseIn)
	{
		Response = ResponseIn;
	}
};

USTRUCT()
struct SEQUENCEPLUGIN_API FDefaultTransak
{
	GENERATED_USTRUCT_BODY()
public:
	const static inline FString FiatCurrency = "USD";
	const static inline FString FiatAmount = "50";
	const static inline FString CryptoCurrency = "USDC";
	const static inline FString Networks = "ethereum,mainnet,arbitrum,optimism,polygon,polygonzkevm,zksync,base,bnb,oasys,astar,avaxcchain";
	static inline constexpr bool DisableWalletAddressForm = true;
};

USTRUCT()
struct SEQUENCEPLUGIN_API FAddFundsSettings
{
	GENERATED_USTRUCT_BODY()
public:
	FString WalletAddress = "";
	FString FiatCurrency = "";
	FString DefaultFiatAmount = "";
	FString DefaultCryptoCurrency = "USDC";
	FString Networks = "";
	
	FAddFundsSettings(){}

	FAddFundsSettings(const FString& WalletAddressIn, const FString& FiatCurrencyIn = FDefaultTransak::FiatCurrency, const FString& DefaultFiatAmountIn = FDefaultTransak::FiatAmount, const FString& DefaultCryptoCurrencyIn = FDefaultTransak::CryptoCurrency, const FString& NetworksIn = FDefaultTransak::Networks)
	{
		WalletAddress = WalletAddressIn;
		FiatCurrency = FiatCurrencyIn;
		DefaultFiatAmount = DefaultFiatAmountIn;
		DefaultCryptoCurrency = DefaultCryptoCurrencyIn;
		Networks = NetworksIn;
	}	
};

USTRUCT()
struct SEQUENCEPLUGIN_API FOnOffQueryParameters
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

	FOnOffQueryParameters(const FString& WalletAddressIn, const FAddFundsSettings& AddFundSettingsIn, bool DisableWalletAddressFormIn = FDefaultTransak::DisableWalletAddressForm)
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