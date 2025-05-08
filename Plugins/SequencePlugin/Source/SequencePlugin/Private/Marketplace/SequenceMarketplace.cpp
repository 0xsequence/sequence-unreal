// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Marketplace/SequenceMarketplace.h"
#include "Util/Async.h"
#include "JsonObjectConverter.h"
#include "Http.h"
#include "Util/SequenceSupport.h"
#include "Util/Log.h"
#include "ConfigFetcher.h"
#include "HttpManager.h"
#include "Indexer/SequenceIndexer.h"
#include "Marketplace/Structs/SeqGetCollectibleOrderArgs.h"
#include "Marketplace/Structs/SeqGetCollectibleOrderReturn.h"
#include "Marketplace/Structs/SeqGetFloorOrderArgs.h"
#include "Marketplace/Structs/SeqGetOrderReturn.h"
#include "Marketplace/Structs/SeqGetSwapPriceArgs.h"
#include "Marketplace/Structs/SeqListCollectibleListingsArgs.h"
#include "Marketplace/Structs/SeqListCollectibleOffersReturn.h"
#include "Marketplace/Structs/SeqListCurrenciesReturn.h"

USequenceMarketplace::USequenceMarketplace(){}

FString USequenceMarketplace::Url(const int64& ChainID, const FString& EndPoint) const
{
	FString Out_URL = HostName(ChainID);
	Out_URL.Append(this->PATH);

	if (EndPoint.Len() > 0)
	{
		Out_URL.Append(EndPoint);
	}
	return Out_URL;
}

FString USequenceMarketplace::HostName(const int64 ChainID)
{
	FString Hostname = "https://";
	Hostname.Append("marketplace-api.sequence.app/");
	Hostname.Append(USequenceSupport::GetNetworkNameForUrl(ChainID));
	return Hostname;
}



void USequenceMarketplace::HTTPPost(const int64& ChainID, const FString& Endpoint, const FString& Args, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString RequestURL = this->Url(ChainID, Endpoint);

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
	 
	FString CurlCommand = FString::Printf(
		TEXT("curl -X %s \"%s\" -H \"Content-Type: application/json\" -H \"Accept: application/json\" -H \"X-Access-Key: %s\" --data \"%s\""),
		*HTTP_Post_Req->GetVerb(),
		*HTTP_Post_Req->GetURL(),
		*HTTP_Post_Req->GetHeader("X-Access-Key"),
		*FString(UTF8_TO_TCHAR(HTTP_Post_Req->GetContent().GetData())).Replace(TEXT("\""), TEXT("\\\""))
	);

	SEQ_LOG_EDITOR(Display, TEXT("%s"), *CurlCommand);

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

template < typename T> FString USequenceMarketplace::BuildArgs(T StructIn)
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

//generic
template<typename T> T USequenceMarketplace::BuildResponse(const FString Text)
{
	//Take the FString and convert it to a JSON object first!
	TSharedPtr<FJsonObject> JSON_Step;
	//Then take the json object we make and convert it to a USTRUCT of type T then we return that!
	T Ret_Struct;

	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), JSON_Step))
	{
		SEQ_LOG(Display, TEXT("Failed to convert String: %s to Json object"), *Text);
		return T();
	}
	//this next line with throw an exception in null is used as an entry in json attributes! we need to remove null entries
	if (Ret_Struct.customConstructor)
	{//use the custom constructor!
		Ret_Struct.construct(*JSON_Step.Get());
	}
	else
	{//use unreal parsing!
		if (!FJsonObjectConverter::JsonObjectToUStruct<T>(JSON_Step.ToSharedRef(), &Ret_Struct))
		{
			SEQ_LOG(Display, TEXT("Failed to convert Json Object: %s to USTRUCT of type T"), *Text);
			return T();
		}
	}
	Ret_Struct.Setup(*JSON_Step.Get());//now for the edge cases we will manually inject the data where it needs to be!
	return Ret_Struct;
}

void USequenceMarketplace::ListCurrencies(const int64 ChainID, TSuccessCallback<FSeqListCurrenciesReturn> OnSuccess,
	const FFailureCallback& OnFailure)
{
	const FString Endpoint = "ListCurrencies";
	const FString Args = "";
	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
		{
			const FSeqListCurrenciesReturn Response = this->BuildResponse<FSeqListCurrenciesReturn>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

void USequenceMarketplace::GetCollectiblesWithLowestListingsFirst(const int64 ChainID, const FString& ContractAddress,
	const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page, TSuccessCallback<FSeqListCollectiblesReturn> OnSuccess,
	const FFailureCallback& OnFailure)
{
	const FString Endpoint = "ListCollectibles";
	const FString Args = BuildArgs<FSeqListCollectiblesArgs>(FSeqListCollectiblesArgs{LISTING, ContractAddress, Filter, Page});
	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
		{
			const FSeqListCollectiblesReturn Response = this->BuildResponse<FSeqListCollectiblesReturn>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

void USequenceMarketplace::GetAllCollectiblesWithLowestListingsFirst(const int64 ChainID,
	const FString& ContractAddress, const FSeqCollectiblesFilter& Filter,
	TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Args = BuildArgs<FSeqListCollectiblesArgs>(FSeqListCollectiblesArgs{LISTING, ContractAddress, Filter, FSeqMarketplacePage::Empty()});
	OrderArray.Empty();

	return ListAllCollectibleListingsWithLowestPriceListingsFirstHelper(ChainID, ContractAddress, Filter, FSeqMarketplacePage::Empty(), [this, OnSuccess](const TArray<FSeqCollectibleOrder>& Orders, const bool IsDone)
	{
		OrderArray.Append(Orders);

		if(IsDone)
		{
			OnSuccess(OrderArray);
		}
	}, OnFailure);
}

void USequenceMarketplace::GetCollectiblesWithHighestPricedOffersFirst(
	const int64 ChainID,
	const FString& ContractAddress,
	const FSeqCollectiblesFilter& Filter,
	const FSeqMarketplacePage& Page,
	TSuccessCallback<FSeqListCollectiblesReturn> OnSuccess,
	const FFailureCallback& OnFailure)
{
	const FString Endpoint = "ListCollectibles";
	const FString Args = BuildArgs<FSeqListCollectiblesArgs>(FSeqListCollectiblesArgs{OFFER, ContractAddress, Filter, Page});
	HTTPPost(ChainID, Endpoint, Args, [this, OnSuccess](const FString& Content)
		{
			const FSeqListCollectiblesReturn Response = this->BuildResponse<FSeqListCollectiblesReturn>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

void USequenceMarketplace::ListAllCollectibleOffersWithHighestPricedOfferFirst(
	const int64 ChainID,
	const FString& ContractAddress,
	const FSeqCollectiblesFilter& Filter,
	TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess,
	const FFailureCallback& OnFailure)
{
	const FString Args = BuildArgs<FSeqListCollectiblesArgs>(FSeqListCollectiblesArgs{LISTING, ContractAddress, Filter, FSeqMarketplacePage::Empty()});
	OrderArray.Empty();

	return ListAllCollectibleOffersWithHighestPricedOfferFirstHelper(ChainID, ContractAddress, Filter, FSeqMarketplacePage::Empty(), [this, OnSuccess](const TArray<FSeqCollectibleOrder>& Orders, const bool IsDone)
	{
		OrderArray.Append(Orders);

		if(IsDone)
		{
			OnSuccess(OrderArray);
		}
	}, OnFailure);
}

void USequenceMarketplace::GetLowestPriceOfferForCollectible(
	const int64 ChainID,
	const FString& ContractAddress,
	const FString& TokenID,
	const FSeqCollectiblesFilter& Filter,
	const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess,
	const FFailureCallback& OnFailure)
{
	const FString Endpoint = "GetLowestPriceOfferForCollectible";
	const FSeqGetCollectibleOrderArgs Args = FSeqGetCollectibleOrderArgs{ ContractAddress, TokenID, Filter};
	GetCollectibleOrder(ChainID, Endpoint, Args, OnSuccess, OnFailure);
}

void USequenceMarketplace::GetHighestPriceOfferForCollectible(const int64 ChainID, const FString& ContractAddress,
	const FString& TokenID, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess,
	const FFailureCallback& OnFailure)
{
	const FString Endpoint = "GetHighestPriceOfferForCollectible";
	const FSeqGetCollectibleOrderArgs Args = FSeqGetCollectibleOrderArgs{ContractAddress, TokenID, Filter};
	GetCollectibleOrder(ChainID, Endpoint, Args, OnSuccess, OnFailure);
}

void USequenceMarketplace::GetLowestPriceListingForCollectible(const int64 ChainID, const FString& ContractAddress,
	const FString& TokenID, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess,
	const FFailureCallback& OnFailure)
{
	const FString Endpoint = "GetLowestPriceListingForCollectible";
	const FSeqGetCollectibleOrderArgs Args = FSeqGetCollectibleOrderArgs{ContractAddress, TokenID, Filter};
	GetCollectibleOrder(ChainID, Endpoint, Args, OnSuccess, OnFailure);
}

void USequenceMarketplace::GetHighestPriceListingForCollectible(const int64 ChainID, const FString& ContractAddress,
	const FString& TokenID, const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess,
	const FFailureCallback& OnFailure)
{
	const FString Endpoint = "GetHighestPriceListingForCollectible";
	const FSeqGetCollectibleOrderArgs Args = FSeqGetCollectibleOrderArgs{ContractAddress, TokenID, Filter};
	GetCollectibleOrder(ChainID, Endpoint, Args, OnSuccess, OnFailure);
}

void USequenceMarketplace::ListListingsForCollectible(const int64 ChainID, const FString& ContractAddress,
	const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page,
	TSuccessCallback<FSeqListCollectibleListingsReturn> OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Endpoint = "ListListingsForCollectible";
	const FSeqListCollectibleListingsArgs Args = FSeqListCollectibleListingsArgs{ContractAddress, TokenID, Filter, Page};
	HTTPPost(ChainID, Endpoint, BuildArgs(Args), [this, OnSuccess](const FString& Content)
		{
			const FSeqListCollectibleListingsReturn Response = this->BuildResponse<FSeqListCollectibleListingsReturn>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

void USequenceMarketplace::ListAllListingsForCollectible(const int64 ChainID,
	const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter,
	TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Args = BuildArgs<FSeqListCollectibleListingsArgs>(FSeqListCollectibleListingsArgs{ ContractAddress, TokenID, Filter, FSeqMarketplacePage::Empty()});
	OrderArray.Empty();

	return ListAllListingsForCollectibleHelper(ChainID, ContractAddress, TokenID, Filter, FSeqMarketplacePage::Empty(), [this, OnSuccess](const TArray<FSeqCollectibleOrder>& Orders, const bool IsDone)
	{
		OrderArray.Append(Orders);

		if(IsDone)
		{
			OnSuccess(OrderArray);
		}
	}, OnFailure);
}

void USequenceMarketplace::ListOffersForCollectible(const int64 ChainID, const FString& ContractAddress, const FString& TokenID,
	const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page,
	TSuccessCallback<FSeqListCollectibleOffersReturn> OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Endpoint = "ListOffersForCollectible";
	const FSeqListCollectibleListingsArgs Args = FSeqListCollectibleListingsArgs{ContractAddress, TokenID, Filter, Page};
	HTTPPost(ChainID, Endpoint, BuildArgs(Args), [this, OnSuccess](const FString& Content)
		{
			const FSeqListCollectibleOffersReturn Response = this->BuildResponse<FSeqListCollectibleOffersReturn>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

void USequenceMarketplace::ListAllOffersForCollectible(const int64 ChainID, const FString& ContractAddress, const FString& TokenID, const FSeqCollectiblesFilter& Filter, TSuccessCallback<TArray<FSeqCollectibleOrder>> OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Args = BuildArgs<FSeqListCollectibleListingsArgs>(FSeqListCollectibleListingsArgs{ ContractAddress, TokenID, Filter, FSeqMarketplacePage::Empty()});
	OrderArray.Empty();

	return ListAllOffersForCollectibleHelper(ChainID, ContractAddress, TokenID, Filter, FSeqMarketplacePage::Empty(), [this, OnSuccess](const TArray<FSeqCollectibleOrder>& Orders, bool IsDone)
	{
		OrderArray.Append(Orders);

		if(IsDone)
		{
			OnSuccess(OrderArray);
		}
	}, OnFailure);
}

void USequenceMarketplace::GetFloorOrder(const int64 ChainID, const FString& ContractAddress,
	const FSeqCollectiblesFilter& Filter, const TSuccessCallback<FSeqCollectibleOrder>& OnSuccess,
	const FFailureCallback& OnFailure)
{
	const FString Endpoint = "GetFloorOrder";
	const FSeqGetFloorOrderArgs Args = FSeqGetFloorOrderArgs{ContractAddress, Filter, FSeqMarketplacePage::Empty()};
	HTTPPost(ChainID, Endpoint, BuildArgs(Args), [this, OnSuccess](const FString& Content)
			{
				const FSeqGetOrderReturn Response = this->BuildResponse<FSeqGetOrderReturn>(Content);
				OnSuccess(Response.Collectible);
			}, OnFailure);
}

// HELPER FUNCTIONS
void USequenceMarketplace::ListAllCollectibleListingsWithLowestPriceListingsFirstHelper(
	const int64 ChainID,
	const FString& ContractAddress,
	const FSeqCollectiblesFilter& Filter,
	const FSeqMarketplacePage& Page,
	TFunction<void(TArray<FSeqCollectibleOrder>, bool)> OnSuccess,
	const FFailureCallback& OnFailure)
{
	GetCollectiblesWithLowestListingsFirst(ChainID, ContractAddress, Filter, Page, [ChainID, ContractAddress, Filter, this, OnSuccess, OnFailure](const FSeqListCollectiblesReturn& CollectiblesReturn)
	{
		if (CollectiblesReturn.Page.More)
		{
			// Call the next page
			ListAllCollectibleListingsWithLowestPriceListingsFirstHelper(ChainID, ContractAddress, Filter, CollectiblesReturn.Page, OnSuccess, OnFailure);
			OnSuccess(CollectiblesReturn.CollectibleOrders, false);
		}
		else
		{
			OnSuccess(CollectiblesReturn.CollectibleOrders, true);
		}
	}, OnFailure);
}

void USequenceMarketplace::ListAllCollectibleOffersWithHighestPricedOfferFirstHelper(
	const int64 ChainID,
	const FString& ContractAddress,
	const FSeqCollectiblesFilter& Filter,
	const FSeqMarketplacePage& Page,
	TFunction<void(TArray<FSeqCollectibleOrder>, bool)> OnSuccess,
	const FFailureCallback& OnFailure)
{
	GetCollectiblesWithHighestPricedOffersFirst(ChainID, ContractAddress, Filter, Page, [ChainID, ContractAddress, Filter, this, OnSuccess, OnFailure](const FSeqListCollectiblesReturn& CollectiblesReturn)
	{
		
		if (CollectiblesReturn.Page.More)
		{
			// Call the next page
			ListAllCollectibleOffersWithHighestPricedOfferFirstHelper(ChainID, ContractAddress, Filter, CollectiblesReturn.Page, OnSuccess, OnFailure);
			OnSuccess(CollectiblesReturn.CollectibleOrders, false);
		}
		else
		{
			OnSuccess(CollectiblesReturn.CollectibleOrders, true);
		}
	}, OnFailure);
}

void USequenceMarketplace::GetCollectibleOrder(const int64 ChainID, const FString& Endpoint, const FSeqGetCollectibleOrderArgs& Args, TSuccessCallback<FSeqCollectibleOrder> OnSuccess,
	const FFailureCallback& OnFailure)
{
	HTTPPost(ChainID, Endpoint, BuildArgs<FSeqGetCollectibleOrderArgs>(Args), [this, OnSuccess](const FString& Content)
		{
			const FSeqGetCollectibleOrderReturn Response = this->BuildResponse<FSeqGetCollectibleOrderReturn>(Content);
			OnSuccess(Response.Order);
		}, OnFailure);
}

void USequenceMarketplace::ListAllListingsForCollectibleHelper(const int64 ChainID, const FString& ContractAddress,
	const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page,
	TFunction<void(TArray<FSeqCollectibleOrder>, bool)> OnSuccess, const FFailureCallback& OnFailure)
{
	ListListingsForCollectible(ChainID, ContractAddress, TokenID, Filter, Page, [ChainID, ContractAddress, Filter, TokenID, this, OnSuccess, OnFailure](const FSeqListCollectibleListingsReturn& CollectiblesReturn)
	{
		
		if (CollectiblesReturn.Page.More)
		{
			// Call the next page
			ListAllListingsForCollectibleHelper(ChainID, ContractAddress, TokenID, Filter, CollectiblesReturn.Page, OnSuccess, OnFailure);
			OnSuccess(CollectiblesReturn.Listings, false);
		}
		else
		{
			OnSuccess(CollectiblesReturn.Listings, true);
		}
	}, OnFailure);
}

void USequenceMarketplace::ListAllOffersForCollectibleHelper(const int64 ChainID, const FString& ContractAddress,
	const FString& TokenID, const FSeqCollectiblesFilter& Filter, const FSeqMarketplacePage& Page,
	TFunction<void(TArray<FSeqCollectibleOrder>, bool)> OnSuccess, const FFailureCallback& OnFailure)
{
	ListOffersForCollectible(ChainID, ContractAddress, TokenID, Filter, Page, [ChainID, ContractAddress, Filter, TokenID, this, OnSuccess, OnFailure](const FSeqListCollectibleOffersReturn& CollectiblesReturn)
	{
		
		if (CollectiblesReturn.Page.More)
		{
			// Call the next page
			ListAllOffersForCollectibleHelper(ChainID, ContractAddress, TokenID, Filter, CollectiblesReturn.Page, OnSuccess, OnFailure);
			OnSuccess(CollectiblesReturn.Offers, false);
		}
		else
		{
			OnSuccess(CollectiblesReturn.Offers, true);
		}
	}, OnFailure);
}
