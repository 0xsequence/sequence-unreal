// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.


#include "Marketplace/Sardine/SardineCheckout.h"

#include "ConfigFetcher.h"
#include "HttpModule.h"
#include "Checkout/SequenceCheckout.h"
#include "Checkout/Structs/TransactionStep.h"
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
#include "Types/SaleDetails.h"
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
		OnSuccess(TTuple<FString, long>("USDC", 6)); // TODO: Use commented logic once ABI decoding is fixed
		
		// UERC20* PaymentTokenContract = NewObject<UERC20>();
		// PaymentTokenContract->Initialize(PaymentToken);
		// FContractCall SymbolCall = PaymentTokenContract->MakeSymbolTransaction();
		// _wallet->Call(SymbolCall, [this, PaymentTokenContract, OnSuccess, OnFailure](FUnsizedData Data)
		// {
		// 	FString PaymentTokenSymbol = UTF8ToString(Data);
		// 	FContractCall DecimalsCall = PaymentTokenContract->MakeDecimalsTransaction();
		// 	_wallet->Call(DecimalsCall, [this, PaymentTokenSymbol, OnSuccess](FUnsizedData Data)
		// 	{
		// 		long PaymentTokenDecimals = FCString::Atoi64(*UTF8ToString(Data));
		// 		OnSuccess(TTuple<FString, long>(PaymentTokenSymbol, PaymentTokenDecimals));
		// 	}, OnFailure);
		// }, OnFailure);
	}
}

void USardineCheckout::GetSaleDetails(const FContractCall& SaleDetailsCall, long& Amount, long TokenID, TSuccessCallback<FSaleDetails> OnSuccess, const FFailureCallback& OnFailure)
{
	_wallet->Call(SaleDetailsCall, [this, Amount, TokenID, OnSuccess, OnFailure](FUnsizedData Data)
	{
		FSaleDetails SaleDetails;
		SaleDetails.Initialize(Data);

		if (Amount > SaleDetails.SupplyCap)
		{
			SaleDetails.Amount = SaleDetails.SupplyCap;
		}
		else
		{
			SaleDetails.Amount = Amount;
		}
		if (SaleDetails.StartTimeLong > FDateTime::UtcNow().ToUnixTimestamp())
		{
			OnFailure(FSequenceError(RequestFail, FString::Printf(TEXT("Token id %d is not yet available for sale"), (int) TokenID)));
		}
		if (SaleDetails.EndTimeLong < FDateTime::UtcNow().ToUnixTimestamp())
		{
			OnFailure(FSequenceError(RequestFail, FString::Printf(TEXT("Token id %d is no longer available for sale"), (int) TokenID)));
		}

		OnSuccess(SaleDetails);
	}, OnFailure);
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

	UE_LOG(LogTemp, Warning, TEXT("curl command: %s"), *CurlCommand);
	// SEQ_LOG_EDITOR(Log, TEXT("%s"), *CurlCommand);


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

void USardineCheckout::SardineGetNFTCheckoutToken(int64 ChainId, const FString& WalletAddress, TArray<FSeqCollectibleOrder> Orders, long Quantity,
	FString RecipientAddress, TArray<FAdditionalFee> AdditionalFee, FString MarketPlaceContractAddress,
	TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure)
{
	if (RecipientAddress.IsEmpty())
	{
		RecipientAddress = WalletAddress;
	}
	
	if(Orders.Num() < 1)
	{
		OnFailure(FSequenceError(InvalidArgument, "Orders must contain at least one collectible"));
		return;
	}

	FSeqCollectibleOrder Order = Orders[0];

	if(Order.Order.PriceCurrencyAddress == FAddress::New().ToHex())
	{
		OnFailure(FSequenceError(InvalidArgument, "Sardine doesn't support native currency checkout; please choose a different payment token"));
		return;
	}

	FString PriceSymbol = USequenceSupport::GetNetworkSymbol(ChainId);

	const USequenceCheckout* Checkout = NewObject<USequenceCheckout>();

	TArray<FSeqOrder> BuyOrders;
	for(FSeqCollectibleOrder CollectibleOrder : Orders)
		BuyOrders.Push(CollectibleOrder.Order);

	Checkout->GenerateBuyTransaction(WalletAddress, BuyOrders, Quantity, AdditionalFee[0], EWalletKind::Unknown, 
	([this, OnSuccess, ChainId, RecipientAddress, MarketPlaceContractAddress, Quantity, PriceSymbol, Order, OnFailure](const FGenerateTransactionResponse& Response)
	{
		const FTransactionStep Step = FTransactionStep::ExtractBuyStep(Response.Steps);

		const FSardineGetNFTCheckoutTokenArgs Args {
			"",
			3600,
			FSardinePaymentMethodTypeConfig {
				PaymentMethods,
				PaymentMethod_Us_Debit
			},
			Order.TokenMetadata.name,
			Order.TokenMetadata.image,
			USequenceSupport::GetNetworkNameForUrl(ChainId),
			RecipientAddress,
			MarketPlaceContractAddress,
			"calldata_execution",
			"smart_contract",
			Order.Order.TokenId,
			Quantity,
			Order.Order.QuantityDecimals,
			Order.Order.PriceAmount,
			Order.Order.PriceCurrencyAddress,
			PriceSymbol,
			Order.Order.PriceDecimals,
			Step.Data,
		};

		FString Endpoint = "SardineGetNFTCheckoutToken";
		HTTPPost(Endpoint, BuildArgs<FSardineGetNFTCheckoutTokenArgs>(Args), [this, OnSuccess](const FString& Content)
			{
				const FSardineGetNFTCheckoutTokenResponse Response = this->BuildResponse<FSardineGetNFTCheckoutTokenResponse>(Content);
				OnSuccess(Response.Checkout);
			}, OnFailure);
		
	}), OnFailure);
}

void USardineCheckout::SardineGetNFTCheckoutToken(int64 ChainId, UERC1155SaleContract* SaleContract, FString CollectionAddress,
	long TokenID, long Amount, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure,
	FString RecipientAddress, TArray<uint8> data, TArray<FString> Proof)
{
	FString RecipientAddressToUse = RecipientAddress;
	if (RecipientAddressToUse.IsEmpty())
	{
		RecipientAddressToUse = _wallet->GetWalletAddress();
	}
	
	if(Amount < 1)
	{
		OnFailure(FSequenceError(InvalidArgument, "Amount must be greater than 0"));
		return;
	}

	// set provider url to appropriate node gateway url for given chain
	_wallet->UpdateProviderURL(ChainId);

	FString PaymentToken = SaleContract->PaymentToken;

	GetPaymentDetails(PaymentToken, [this, PaymentToken, SaleContract, CollectionAddress, TokenID, ChainId, RecipientAddress, &Amount, Proof, OnSuccess, OnFailure](TTuple<FString, long> PaymentDetails)
	{
		FString PaymentTokenSymbol = PaymentDetails.Get<0>();
		long PaymentTokenDecimals = PaymentDetails.Get<1>();

		UMarketplace* Marketplace = NewObject<UMarketplace>();
		Marketplace->GetCollectible(ChainId, CollectionAddress, FString::FromInt(TokenID),
			[this, SaleContract, TokenID, ChainId, RecipientAddress, Amount, PaymentToken, PaymentTokenSymbol, PaymentTokenDecimals, Proof, OnSuccess, OnFailure](const FSeqTokenMetaData& TokenMetaData)
		{
			FContractCall SaleDetailsCall = SaleContract->GetTokenSaleDetails(TokenID);
			TSuccessCallback<FSaleDetails> OnGetSaleDetailsSuccess = [this, TokenID, Amount, SaleContract, RecipientAddress, Proof, TokenMetaData, ChainId, PaymentToken, PaymentTokenSymbol, PaymentTokenDecimals, OnSuccess, OnFailure](FSaleDetails SaleDetails)
			{
				TArray<int32> TokenIds { (int32)TokenID };
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
					USequenceSupport::GetNetworkNameForUrl(ChainId),
					RecipientAddress,
					SaleContract->ContractAddress,
					"calldata_execution",
					"smart_contract",
					FString::FromInt(TokenID),
					SaleDetails.Amount,
					(uint)TokenMetaData.decimals,
					FString::FromInt(SaleDetails.Cost),
					PaymentToken,
					PaymentTokenSymbol,
					PaymentTokenDecimals,
					CallData
				};

				SardineGetNFTCheckoutToken(Args, OnSuccess, OnFailure);
			};

			FSaleDetails MockSaleDetails {
				"",
				20000,
				9999999999,
				1723058097,
				2448832492,
				Amount,
				TArray<uint8>()
			};

			OnGetSaleDetailsSuccess(MockSaleDetails);
			// TODO: Use helper below instead
			// GetSaleDetails(SaleDetailsCall, Amount, TokenID, OnGetSaleDetailsSuccess, OnFailure);
		}, OnFailure);
	}, OnFailure);
}

void USardineCheckout::SardineGetNFTCheckoutToken(int64 ChainID, UERC721SaleContract* SaleContract, FString CollectionAddress,
	long TokenID, long Amount, TSuccessCallback<FSardineNFTCheckout> OnSuccess, const FFailureCallback& OnFailure,
	FString RecipientAddress, TArray<uint8> data, TArray<FString> Proof)
{
	if (RecipientAddress.IsEmpty())
	{
		RecipientAddress = _wallet->GetWalletAddress();
	}

	if (Amount < 1)
	{
		OnFailure(FSequenceError(InvalidArgument, "Amount must be greater than 0"));
		return;
	}

	// set provider url to appropriate node gateway url for given chain
	_wallet->UpdateProviderURL(ChainID);

	UMarketplace* Marketplace = NewObject<UMarketplace>();
	Marketplace->GetCollectible(ChainID, CollectionAddress, FString::FromInt(TokenID),[this, SaleContract, RecipientAddress, Amount, TokenID, Proof, ChainID, OnSuccess, OnFailure](const FSeqTokenMetaData& TokenMetaData)
	{
		FContractCall SaleDetailsCall = SaleContract->GetSaleDetails();
		TSuccessCallback<FSaleDetails> OnGetSaleDetailsSuccess = [this, SaleContract, RecipientAddress, Amount, Proof, TokenMetaData, ChainID, TokenID, OnSuccess, OnFailure](FSaleDetails SaleDetails)
		{
			FString PaymentToken = SaleDetails.PaymentToken;
			if (PaymentToken == FAddress::New().ToHex())
			{
				OnFailure(FSequenceError(InvalidArgument, "Sardine checkout does not support native currency checkout; please choose an sales contract with a different payment token"));
			}

			GetPaymentDetails(PaymentToken, [this, SaleContract, RecipientAddress, Amount, Proof, TokenMetaData, ChainID, TokenID, SaleDetails, PaymentToken, OnSuccess, OnFailure](TTuple<FString, long> PaymentDetails)
			{
				FString CallData = SaleContract->MakePurchaseTransaction(RecipientAddress, Amount, Proof).data;

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
					FString::FromInt(TokenID),
					SaleDetails.Amount,
					(uint)TokenMetaData.decimals,
					FString::FromInt(SaleDetails.Cost),
					PaymentToken,
					PaymentDetails.Get<0>(),
					PaymentDetails.Get<1>(),
					CallData
				};

				SardineGetNFTCheckoutToken(Args, OnSuccess, OnFailure);
			}, OnFailure);
		};

		FSaleDetails MockSaleDetails {
			"0x3c499c542cEF5E3811e1192ce70d8cC03d5c3359",
			0,
			10000,
			1723058097,
			2448832492,
			Amount,
			TArray<uint8>()
		};

		OnGetSaleDetailsSuccess(MockSaleDetails);

		// TODO: Use helper below instead:
		// GetSaleDetails(SaleDetailsCall, Amount, TokenID, , OnFailure);
	}, OnFailure);
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

void USardineCheckout::Checkout(FSardineNFTCheckout Token)
{
	FString URL = CheckoutURL(Token);
	FString * ErrorPtr = nullptr;
	FPlatformProcess::LaunchURL(*URL,TEXT(""),ErrorPtr);
	if (ErrorPtr)
	{
		UE_LOG(LogTemp,Error,TEXT("Browser LaunchError: %s"), **ErrorPtr);
	}
}


