// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Marketplace/Marketplace.h"
#include "Util/Async.h"
#include "JsonObjectConverter.h"
#include "Http.h"
#include "Util/SequenceSupport.h"
#include "Util/Log.h"
#include "ConfigFetcher.h"
#include "HttpManager.h"


UMarketplace::UMarketplace(){}

FString UMarketplace::Url(const int64& ChainID, const FString& EndPoint) const
{
	FString Out_URL = HostName(ChainID);
	Out_URL.Append(this->PATH);

	if (EndPoint.Len() > 0)
	{
		Out_URL.Append(EndPoint);
	}
	return Out_URL;
}

FString UMarketplace::HostName(const int64 ChainID)
{
	FString Hostname = "https://";
	Hostname.Append("marketplace-api.sequence.app/");
	Hostname.Append(USequenceSupport::GetNetworkNameForUrl(ChainID));
	return Hostname;
}

void UMarketplace::HTTPPost(const int64& ChainID, const FString& Endpoint, const FString& Args, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
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

	SEQ_LOG_EDITOR(Log, TEXT("%s"), *CurlCommand);


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

void UMarketplace::GetCollectiblesWithLowestListings(const int64 ChainID, const FSeqGetCollectiblesWithLowestListingsArgs& Args, TSuccessCallback<FSeqGetCollectiblesWithLowestListingsReturn> OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Endpoint = "ListCollectiblesWithLowestListing"; 

	HTTPPost(ChainID, Endpoint, BuildArgs<FSeqGetCollectiblesWithLowestListingsArgs>(Args), [this, OnSuccess](const FString& Content)
		{
			const FSeqGetCollectiblesWithLowestListingsReturn Response = this->BuildResponse<FSeqGetCollectiblesWithLowestListingsReturn>(Content);
			OnSuccess(Response);
		}, OnFailure);
}

template < typename T> FString UMarketplace::BuildArgs(T StructIn)
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
template<typename T> T UMarketplace::BuildResponse(const FString Text)
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