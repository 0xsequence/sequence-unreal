// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.


#include "Marketplace/Sardine/SardineCheckout.h"

#include "ConfigFetcher.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Marketplace/Marketplace.h"
#include "Marketplace/Sardine/Structs/SardineEnabledTokensResponse.h"
#include "Marketplace/Sardine/Structs/SardineGetNFTCheckoutTokenResponse.h"
#include "Marketplace/Sardine/Structs/SardineGetQuoteArgs.h"
#include "Marketplace/Sardine/Structs/SardineGetQuoteResponse.h"
#include "Marketplace/Sardine/Structs/SardineSupportedFiatCurrenciesResponse.h"
#include "Marketplace/Sardine/Structs/SardineSupportedRegionsResponse.h"
#include "Marketplace/Sardine/Structs/SardineSupportedTokensResponse.h"
#include "Marketplace/Sardine/Structs/SardineTokenResponse.h"
#include "Types/ERC20.h"
#include "Util/Log.h"

TArray<ESardinePaymentMethod> PaymentMethods {
	PaymentMethod_Us_Debit,
	PaymentMethod_Us_Credit,
	PaymentMethod_International_Debit,
	PaymentMethod_International_Credit,
	PaymentMethod_Ach
};

USardineCheckout::USardineCheckout()
{
	const TOptional<USequenceWallet*> WalletOptional = USequenceWallet::Get();
	if (WalletOptional.IsSet() && WalletOptional.GetValue())
	{
		_wallet = WalletOptional.GetValue();
	}
}

FString USardineCheckout::Url(const FString& EndPoint) const
{
	return _baseUrl + "/" + EndPoint;
}

void USardineCheckout::GetPaymentToken(UERC1155SaleContract* SaleContract, FString CollectionAddress, long TokenID,
	int64 ChainID, FString RecipientAddress, long Amount, const TArray<FString>& Proof, TSuccessCallback<FSardineNFTCheckout> OnSuccess,
	const FFailureCallback& OnFailure)
{
	FString PaymentToken = SaleContract->PaymentToken;

	GetPaymentDetails(PaymentToken, [this, PaymentToken, SaleContract, CollectionAddress, TokenID, ChainID, RecipientAddress, Amount, Proof, OnSuccess, OnFailure](TTuple<FString, long> PaymentDetails)
	{
		FString PaymentTokenSymbol = PaymentDetails.Get<0>();
		long PaymentTokenDecimals = PaymentDetails.Get<1>();

		GetCollectible(PaymentToken, SaleContract, CollectionAddress, TokenID, ChainID, RecipientAddress, Amount, PaymentTokenSymbol, PaymentTokenDecimals, Proof, OnSuccess, OnFailure);
	}, OnFailure);
}

void USardineCheckout::GetPaymentDetails(FString PaymentToken, TSuccessCallback<TTuple<FString, long>> OnSuccess, const FFailureCallback& OnFailure)
{
	if (PaymentToken == FAddress::New().ToHex())
	{
		FString PaymentTokenSymbol = "POL";
		long PaymentTokenDecimals = 18;
		OnSuccess(TTuple<FString, long>(PaymentTokenSymbol, PaymentTokenDecimals));
	}
	else
	{
		UERC20* PaymentTokenContract = NewObject<UERC20>();
		PaymentTokenContract->Initialize(PaymentToken);
		FContractCall SymbolCall = PaymentTokenContract->MakeSymbolTransaction();
		_wallet->Call(SymbolCall, [this, PaymentTokenContract, OnSuccess, OnFailure](FUnsizedData Data)
		{
			FString PaymentTokenSymbol = UTF8ToString(Data);
			FContractCall DecimalsCall = PaymentTokenContract->MakeDecimalsTransaction();
			_wallet->Call(DecimalsCall, [this, PaymentTokenSymbol, OnSuccess](FUnsizedData Data)
			{
				long PaymentTokenDecimals = FCString::Atoi64(*UTF8ToString(Data));
				OnSuccess(TTuple<FString, long>(PaymentTokenSymbol, PaymentTokenDecimals));
			}, OnFailure);
		}, OnFailure);
	}
}

void USardineCheckout::GetCollectible(FString PaymentToken, UERC1155SaleContract* SaleContract,
	FString CollectionAddress, long TokenID, int64 ChainID, FString RecipientAddress, long Amount,
	FString PaymentTokenSymbol, long PaymentTokenDecimals, const TArray<FString>& Proof, TSuccessCallback<FSardineNFTCheckout> OnSuccess,
	const FFailureCallback& OnFailure)
{
	UMarketplace* Marketplace = NewObject<UMarketplace>();
	Marketplace->GetCollectible(ChainID, CollectionAddress, FString::FromInt(TokenID),
		[this, SaleContract, TokenID, ChainID, RecipientAddress, Amount, PaymentToken, PaymentTokenSymbol, PaymentTokenDecimals, Proof, OnSuccess, OnFailure](const FSeqTokenMetaData& TokenMetaData)
	{
		GetSaleDetails(SaleContract, TokenID, TokenMetaData, ChainID, RecipientAddress, Amount, PaymentToken,
			PaymentTokenSymbol, PaymentTokenDecimals, Proof, OnSuccess, OnFailure);
	}, OnFailure);
}

void USardineCheckout::GetSaleDetails(UERC1155SaleContract* SaleContract, long TokenId, FSeqTokenMetaData TokenMetaData,
	int64 ChainID, FString RecipientAddress, long Amount, FString PaymentToken, FString PaymentTokenSymbol,
	long PaymentTokenDecimals, const TArray<FString>& Proof, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure)
{
	long Cost = 20000;
	long SupplyCap = 9999999999999;
	long StartTimeLong = 1723058097;
	long EndTimeLong = 2448832492;

	if (Amount > SupplyCap)
	{
		Amount = SupplyCap;
	}
	if (StartTimeLong > FDateTime::UtcNow().ToUnixTimestamp())
	{
		OnFailure(FSequenceError(RequestFail, FString::Printf(TEXT("Token id %d is not yet available for sale"), (int) TokenId)));
	}
	if (EndTimeLong < FDateTime::UtcNow().ToUnixTimestamp())
	{
		OnFailure(FSequenceError(RequestFail, FString::Printf(TEXT("Token id %d is no longer available for sale"), (int) TokenId)));
	}

	TArray<int32> TokenIds { (int32)TokenId };
	TArray<int32> Amounts { (int32)Amount };
	
	FString CallData = SaleContract->MakePurchaseTransaction(RecipientAddress, TokenIds, Amounts, Proof).data;
	
	FSardinePaymentMethodTypeConfig PaymentConfig {
		PaymentMethods,
		PaymentMethod_Us_Debit
	};

	FSardineGetNFTCheckoutTokenArgs Args {
		"",
		3600,
		PaymentConfig,
		TokenMetaData.name,
		TokenMetaData.image,
		USequenceSupport::GetNetworkNameForUrl(ChainID),
		RecipientAddress,
		SaleContract->ContractAddress,
		"calldata_execution",
		"smart_contract",
		FString::FromInt(TokenId),
		Amount,
		(uint)TokenMetaData.decimals,
		FString::FromInt(Cost),
		PaymentToken,
		PaymentTokenSymbol,
		PaymentTokenDecimals,
		CallData
	};

	SardineGetNFTCheckoutToken(Args, OnSuccess, OnFailure);
}


void USardineCheckout::SardineGetNFTCheckoutToken(const FSardineGetNFTCheckoutTokenArgs& Args,
	TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure)
{
	if(Args.TokenAddress == FAddress::New().ToHex())
	{
		OnFailure(FSequenceError(InvalidArgument, "Sardine doesn't support native currency checkout; please choose a different payment token"));
		return;
	}

	HTTPPost("SardineGetNFTCheckoutToken", BuildArgs<FSardineGetNFTCheckoutTokenArgs>(Args), [this, OnSuccess](const FString& Content)
		{
			const FSardineGetNFTCheckoutTokenResponse Response = this->BuildResponse<FSardineGetNFTCheckoutTokenResponse>(Content);
			OnSuccess(Response.Checkout);
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
	const FString Endpoint = "SardineGetNFTCheckoutToken";

	FString CheckWhiteListStatusUrl = Url(Endpoint);
	FString ReferenceId = "sequence-unity-sardine-whitelist-check";
	FString Name = "whitelist-check";
	FString ImageUrl = "https://www.sequence.market/images/placeholder.png";
	int64 ChainID = _wallet->GetNetworkId();
	FString Platform = "calldata_execution";
	FString ExecutionType = "smart_contract";
	FString BlockchainNFTId = "42";
	uint Quantity = 1;
	uint Decimals = 0;
	FString TokenAmount = "1000000";
	FString TokenAddress = "0xa0b86991c6218b36c1d19d4a2e9eb0ce3606eb48";
	FString TokenSymbol = "USDC";
	uint TokenDecimals = 6;
	FString CallData = "0x1";

	FSardinePaymentMethodTypeConfig PaymentConfig {
		PaymentMethods,
		ESardinePaymentMethod::PaymentMethod_Us_Debit
	};

	FSardineGetNFTCheckoutTokenArgs Args {
		ReferenceId,
		3600,
		PaymentConfig,
		Name,
		ImageUrl,
		USequenceSupport::GetNetworkName(ChainID),
		FAddress::New().ToHex(),
		Address,
		Platform,
		ExecutionType,
		BlockchainNFTId,
		Quantity,
		Decimals,
		TokenAmount,
		TokenAddress,
		TokenSymbol,
		TokenDecimals,
		CallData
	};

	HTTPPost(Endpoint, BuildArgs<FSardineGetNFTCheckoutTokenArgs>(Args), [this, OnSuccess](const FString& Content)
		{
			OnSuccess(true);
		}, OnFailure);
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
	const FString Endpoint = "SardineGetClientToken";
	HTTPPost(Endpoint, "", [this, OnSuccess](const FString& Content)
		{
			const FSardineTokenResponse Response = this->BuildResponse<FSardineTokenResponse>(Content);
			OnSuccess(Response.Token);
		}, OnFailure);
}

void USardineCheckout::OnRamp(const FString& ClientToken)
{
	FString URL = CheckoutURL(ClientToken);
	FString * ErrorPtr = nullptr;
	FPlatformProcess::LaunchURL(*URL,TEXT(""),ErrorPtr);
	if (ErrorPtr)
	{
		UE_LOG(LogTemp,Error,TEXT("Browser LaunchError: %s"), **ErrorPtr);
	}
}

void USardineCheckout::SardineGetNFTCheckoutToken(TArray<FSeqCollectibleOrder> Orders, long Quantity,
	FString RecipientAddress, TArray<FAdditionalFee> AdditionalFee, FString MarketPlaceContractAddress,
	TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure)
{
	if (RecipientAddress.IsEmpty())
	{
		// set to wallet address
	}
	
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

void USardineCheckout::SardineGetNFTCheckoutToken(int64 ChainId, UERC1155SaleContract* SaleContract, FString CollectionAddress,
	long TokenID, long Amount, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure,
	FString RecipientAddress, TArray<uint8> data, TArray<FString> Proof)
{
	if (RecipientAddress.IsEmpty())
	{
		// set address to wallet address
	}
	
	if(Amount < 1)
	{
		OnFailure(FSequenceError(InvalidArgument, "Amount must be greater than 0"));
		return;
	}
	
	GetPaymentToken(SaleContract, CollectionAddress, TokenID, ChainId, RecipientAddress, Amount, Proof, OnSuccess, OnFailure);
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

FString USardineCheckout::CheckoutURL(FString ClientToken)
{
	return _baseCheckoutUrl + ClientToken + _sardineCheckoutUrlSuffix;
}

FString USardineCheckout::CheckoutURL(FSardineNFTCheckout Token)
{
	return CheckoutURL(Token.Token);
}


