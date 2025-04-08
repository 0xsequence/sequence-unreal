#include "Integrators/SardineCheckoutBP.h"

USardineCheckoutBP::USardineCheckoutBP(): SardineCheckout(NewObject<USardineCheckout>())
{
}

void USardineCheckoutBP::CheckSardineWhiteListStatus(FString Address, FOnCheckSardineWhiteListStatus OnSuccess,
                                                     FOnSardineCheckoutFailure OnFailure)
{
	SardineCheckout->CheckSardineWhiteListStatus(Address, [OnSuccess](bool Response)
	{
		if(OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [OnFailure](FSequenceError Error)
	{
		if(OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void USardineCheckoutBP::SardineGetQuote(FOnSardineGetQuote OnSuccess, FOnSardineCheckoutFailure OnFailure,
	FString WalletAddress, FSardineToken Token, int Amount, FSardineFiatCurrency QuotedCurrency, ESardinePaymentType PaymentType,ESardineQuoteType QuoteType)
{
	SardineCheckout->SardineGetQuote([OnSuccess](FSardineQuote Response)
	{
		if(OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [OnFailure](FSequenceError Error)
	{
		if(OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	}, WalletAddress, Token, Amount, PaymentType, QuotedCurrency, QuoteType);
}

void USardineCheckoutBP::SardineGetClientToken(FOnSardineGetClientToken OnSuccess, FOnSardineCheckoutFailure OnFailure)
{
	SardineCheckout->SardineGetClientToken([OnSuccess](FString Response)
	{
		if(OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [OnFailure](FSequenceError Error)
	{
		if(OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void USardineCheckoutBP::OnRamp(const FString& ClientToken)
{
	SardineCheckout->OnRamp(ClientToken);
}

void USardineCheckoutBP::SardineGetNFTCheckoutTokenOrders(int64 ChainId, const FString& WalletAddress,
	TArray<FSeqCollectibleOrder> Orders, int Quantity, FString RecipientAddress, TArray<FAdditionalFee> AdditionalFee,
	FString MarketPlaceContractAddress, FOnSardineGetNFTCheckoutToken OnSuccess, FOnSardineCheckoutFailure OnFailure)
{
	SardineCheckout->SardineGetNFTCheckoutToken(ChainId, WalletAddress, Orders, Quantity, RecipientAddress, AdditionalFee, MarketPlaceContractAddress,
		[OnSuccess](FSardineNFTCheckout Response)
		{
			if(OnSuccess.IsBound())
			{
				OnSuccess.Execute(Response);
			}
		}, [OnFailure](FSequenceError Error)
		{
			if(OnFailure.IsBound())
			{
				OnFailure.Execute();
			}
		});	
}

void USardineCheckoutBP::SardineGetNFTCheckoutTokenERC1155(int64 ChainId, UERC1155SaleContract* SaleContract,
	FString CollectionAddress, int TokenID, int Amount, FOnSardineGetNFTCheckoutToken OnSuccess,
	FOnSardineCheckoutFailure OnFailure, TArray<uint8> data, TArray<FString> Proof, FString RecipientAddress)
{
	SardineCheckout->SardineGetNFTCheckoutToken(ChainId, SaleContract, CollectionAddress, TokenID, Amount, [OnSuccess](FSardineNFTCheckout Response)
	{
		if(OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [OnFailure](FSequenceError Error)
	{
		if(OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	}, RecipientAddress, data, Proof);
}

void USardineCheckoutBP::SardineGetNFTCheckoutTokenERC721(int64 ChainID, UERC721SaleContract* SaleContract,
	FString CollectionAddress, int TokenID, int Amount, FOnSardineGetNFTCheckoutToken OnSuccess,
	FOnSardineCheckoutFailure OnFailure, FString RecipientAddress, TArray<uint8> data, TArray<FString> Proof)
{
	SardineCheckout->SardineGetNFTCheckoutToken(ChainID, SaleContract, CollectionAddress, TokenID, Amount, [OnSuccess](FSardineNFTCheckout Response)
	{
		if(OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [OnFailure](FSequenceError Error)
	{
		if(OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	}, RecipientAddress, data, Proof);
}

void USardineCheckoutBP::SardineGetNFTCheckoutOrderStatus(FString OrderID,
	FOnSardineGetNFTCheckoutOrderStatus OnSuccess, FOnSardineCheckoutFailure OnFailure)
{
	SardineCheckout->SardineGetNFTCheckoutOrderStatus(OrderID, [OnSuccess](FSardineOrder Response)
	{
		if(OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [OnFailure](FSequenceError Error)
	{
		if(OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void USardineCheckoutBP::SardineGetSupportedRegions(FOnSardineGetSupportedRegions OnSuccess,
	FOnSardineCheckoutFailure OnFailure)
{
	SardineCheckout->SardineGetSupportedRegions([OnSuccess](TArray<FSardineRegion> Response)
	{
		if(OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [OnFailure](FSequenceError Error)
	{
		if(OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void USardineCheckoutBP::SardineGetSupportedFiatCurrencies(FOnSardineGetSupportedFiatCurrencies OnSuccess,
	FOnSardineCheckoutFailure OnFailure)
{
	SardineCheckout->SardineGetSupportedFiatCurrencies([OnSuccess](TArray<FSardineFiatCurrency> Response)
	{
		if(OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [OnFailure](FSequenceError Error)
	{
		if(OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void USardineCheckoutBP::SardineGetSupportedTokens(FOnSardineGetSupportedTokens OnSuccess,
	FOnSardineCheckoutFailure OnFailure)
{
	SardineCheckout->SardineGetSupportedTokens([OnSuccess](TArray<FSardineSupportedToken> Response)
	{
		if(OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [OnFailure](FSequenceError Error)
	{
		if(OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

void USardineCheckoutBP::SardineGetEnabledTokens(FOnSardineGetEnabledTokens OnSuccess,
	FOnSardineCheckoutFailure OnFailure)
{
	SardineCheckout->SardineGetEnabledTokens([OnSuccess](TArray<FSardineEnabledToken> Response)
	{
		if(OnSuccess.IsBound())
		{
			OnSuccess.Execute(Response);
		}
	}, [OnFailure](FSequenceError Error)
	{
		if(OnFailure.IsBound())
		{
			OnFailure.Execute();
		}
	});
}

FString USardineCheckoutBP::CheckoutURL(FString ClientToken)
{
	return SardineCheckout->CheckoutURL(ClientToken);
}

FString USardineCheckoutBP::CheckoutURLFromToken(FSardineNFTCheckout Token)
{
	return SardineCheckout->CheckoutURL(Token);
}

void USardineCheckoutBP::Checkout(FSardineNFTCheckout Token)
{
	SardineCheckout->Checkout(Token);
}






