// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Indexer/Indexer.h"
#include "ConfigFetcher.h"
#include "Util/Async.h"
#include "JsonObjectConverter.h"
#include "Http.h"
#include "Util/SequenceSupport.h"
#include "HttpManager.h"
#include "Util/Log.h"

UIndexer::UIndexer(){}

/*
	Combines <see cref="PATH" and name="name" to suffix on to the base address
*/
FString UIndexer::Url(const int64& ChainID,const FString& EndPoint) const
{
	FString Out_URL = HostName(ChainID);
	Out_URL.Append(this->PATH);
	
	if (EndPoint.Len() > 0)
	{
		Out_URL.Append(EndPoint);
	}
	return Out_URL;
}

/*
	Get hostname directing to specific chainID
*/
FString UIndexer::HostName(const int64 ChainID)
{
	FString Hostname = "https://";
	Hostname.Append(USequenceSupport::GetNetworkNameForUrl(ChainID));
	Hostname.Append("-indexer.sequence.app");
	return Hostname;
}

/*
	Here we construct a post request and parse out a response if valid.
*/void UIndexer::HTTPPost(const int64& ChainID, const FString& Endpoint, const FString& Args, const TSuccessCallback<FString>& OnSuccess, const FFailureCallback& OnFailure) const
{
	const FString Url = *this->Url(ChainID, Endpoint);
	const TSharedRef<IHttpRequest> HTTP_Post_Req = FHttpModule::Get().CreateRequest();
	FString AccessKey = UConfigFetcher::GetConfigVar(UConfigFetcher::ProjectAccessKey);

	HTTP_Post_Req->SetVerb("POST");
	HTTP_Post_Req->SetHeader("Content-Type", "application/json"); // Two differing headers for the request
	HTTP_Post_Req->SetHeader("Accept", "application/json");
	HTTP_Post_Req->SetHeader("X-Access-Key", AccessKey);
	HTTP_Post_Req->SetTimeout(30);
	HTTP_Post_Req->SetURL(Url);
	HTTP_Post_Req->SetContentAsString(Args);

	FString CurlCommand = FString::Printf(
		TEXT("curl -X %s \"%s\" -H \"Content-Type: application/json\" -H \"Accept: application/json\" -H \"X-Access-Key: %s\" --data \"%s\""),
		*HTTP_Post_Req->GetVerb(),
		*HTTP_Post_Req->GetURL(),
		*HTTP_Post_Req->GetHeader("X-Access-Key"),
		*FString::Printf(TEXT("%s"), *FString(UTF8_TO_TCHAR(HTTP_Post_Req->GetContent().GetData())).Replace(TEXT("\""), TEXT("\\\"")))
	);
	SEQ_LOG_EDITOR(Log, TEXT("%s"), *CurlCommand);

	HTTP_Post_Req->OnProcessRequestComplete().BindLambda([OnSuccess, OnFailure](const FHttpRequestPtr& Request, FHttpResponsePtr Response, const bool bWasSuccessful)
		{
			if (bWasSuccessful && Response.IsValid())
			{
				const int32 ResponseCode = Response->GetResponseCode();
				const FString Content = Response->GetContentAsString();

				SEQ_LOG_EDITOR(Log, TEXT("%s"), *Response->GetContentAsString());

				if (ResponseCode >= 200 && ResponseCode < 300 )
				{
					TSharedPtr<FJsonObject> JsonResponse;
					if (FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Content), JsonResponse) && JsonResponse->HasField("error"))
					{
						FString ErrorMessage = JsonResponse->GetStringField("error");
						OnFailure(FSequenceError(RequestFail, "API Error: " + ErrorMessage));
					}
					else
					{
						OnSuccess(Content);
					}
				}
				else
				{
					OnFailure(FSequenceError(RequestFail, FString::Printf(TEXT("HTTP Error: %d. Response: %s"), ResponseCode, *Content)));
				}
			}
			else
			{
				if (Request.IsValid() && Response.IsValid())
				{
					const FString Content = Response->GetContentAsString();
					OnFailure(FSequenceError(RequestFail, "Request failed: " + Content));
				}
				else
				{
					OnFailure(FSequenceError(RequestFail, "Request failed: Invalid Request Pointer"));
				}
			}
		});

	HTTP_Post_Req->ProcessRequest();
}

/*
	Here we take in a struct and convert it straight into a json object String
	@Param (T) Struct_in the struct we are converting to a json object string
	@Return the JSON Object String
*/
template < typename T> FString UIndexer::BuildArgs(T StructIn)
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
			UE_LOG(LogTemp, Display, TEXT("Failed to convert specified UStruct to a json object\n"));
		}
	}
	return Result;
}

//generic
template<typename T> T UIndexer::BuildResponse(const FString Text)
{
	//Take the FString and convert it to a JSON object first!
	TSharedPtr<FJsonObject> JSON_Step;
	//Then take the json object we make and convert it to a USTRUCT of type T then we return that!
	T Ret_Struct;

	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), JSON_Step))
	{
		UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *Text);
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
			UE_LOG(LogTemp, Display, TEXT("Failed to convert Json Object: %s to USTRUCT of type T"), *Text);
			return T();
		}
	}
	Ret_Struct.Setup(*JSON_Step.Get());//now for the edge cases we will manually inject the data where it needs to be!
	return Ret_Struct;
}

void UIndexer::Ping(const int64 ChainID, TSuccessCallback<bool> OnSuccess, const FFailureCallback& OnFailure)
{
	HTTPPost(ChainID, "Ping", "", [this,OnSuccess](const FString& Content) {
		OnSuccess(this->BuildResponse<FSeqPingReturn>(Content).status);
	}, OnFailure);
}

void UIndexer::Version(const int64 ChainID, TSuccessCallback<FSeqVersion> OnSuccess, const FFailureCallback& OnFailure)
{
	HTTPPost(ChainID, "Version", "", [this,OnSuccess](const FString& Content)
	{
		OnSuccess(this->BuildResponse<FSeqVersionReturn>(Content).version);
	}, OnFailure);
}

void UIndexer::RunTimeStatus(const int64 ChainID, TSuccessCallback<FSeqRuntimeStatus> OnSuccess, const FFailureCallback& OnFailure)
{
	HTTPPost(ChainID, "RuntimeStatus", "", [this,OnSuccess](const FString& Content)
	{
		OnSuccess(this->BuildResponse<FSeqRuntimeStatusReturn>(Content).status);
	}, OnFailure);
}

void UIndexer::GetChainID(const int64 ChainID, TSuccessCallback<int64> OnSuccess, const FFailureCallback& OnFailure)
{
	HTTPPost(ChainID, "GetChainID", "", [this,OnSuccess](const FString& Content)
	{
		OnSuccess(this->BuildResponse<FSeqGetChainIDReturn>(Content).chainID);
	}, OnFailure);
}

void UIndexer::GetEtherBalance(const int64 ChainID, FString AccountAddr, TSuccessCallback<FSeqEtherBalance> OnSuccess, const FFailureCallback& OnFailure)
{//since we are given a raw accountAddress we compose the json arguments here to put in the request manually
	FString JSON_Arg = "{\"accountAddress\":\"";
	JSON_Arg.Append(AccountAddr);
	JSON_Arg.Append("\"}");

	HTTPPost(ChainID, "GetEtherBalance", JSON_Arg, [this,OnSuccess](const FString& Content)
	{
		const FSeqGetEtherBalanceReturn Response = this->BuildResponse<FSeqGetEtherBalanceReturn>(Content);
		OnSuccess(Response.balance);
	}, OnFailure);
}

void UIndexer::GetTokenBalances(const int64 ChainID, const FSeqGetTokenBalancesArgs& Args, TSuccessCallback<FSeqGetTokenBalancesReturn> OnSuccess, const FFailureCallback& OnFailure)
{
	const FString Endpoint = "GetTokenBalances";
	HTTPPost(ChainID, Endpoint, BuildArgs<FSeqGetTokenBalancesArgs>(Args), [this,OnSuccess](const FString& Content)
	{
		const FSeqGetTokenBalancesReturn Response = this->BuildResponse<FSeqGetTokenBalancesReturn>(Content);
		OnSuccess(Response);
	}, OnFailure);
}

void UIndexer::GetTokenSupplies(const int64 ChainID, const FSeqGetTokenSuppliesArgs& Args, TSuccessCallback<FSeqGetTokenSuppliesReturn> OnSuccess, const FFailureCallback& OnFailure)
{
	HTTPPost(ChainID, "GetTokenSupplies", BuildArgs<FSeqGetTokenSuppliesArgs>(Args), [this,OnSuccess](const FString& Content)
	{
		OnSuccess(this->BuildResponse<FSeqGetTokenSuppliesReturn>(Content));
	}, OnFailure);
}

void UIndexer::GetTokenSuppliesMap(const int64 ChainID, const FSeqGetTokenSuppliesMapArgs& Args, TSuccessCallback<FSeqGetTokenSuppliesMapReturn> OnSuccess, const FFailureCallback& OnFailure)
{
	HTTPPost(ChainID, "GetTokenSuppliesMap", BuildArgs<FSeqGetTokenSuppliesMapArgs>(Args), [this,OnSuccess](const FString& Content)
	{
		OnSuccess(this->BuildResponse<FSeqGetTokenSuppliesMapReturn>(Content));
	}, OnFailure);
}

void UIndexer::GetBalanceUpdates(const int64 ChainID, const FSeqGetBalanceUpdatesArgs& Args, TSuccessCallback<FSeqGetBalanceUpdatesReturn> OnSuccess, const FFailureCallback& OnFailure)
{
	HTTPPost(ChainID, "GetBalanceUpdates", BuildArgs<FSeqGetBalanceUpdatesArgs>(Args), [this,OnSuccess](const FString& Content)
	{
		OnSuccess(this->BuildResponse<FSeqGetBalanceUpdatesReturn>(Content));
	}, OnFailure);
}

void UIndexer::GetTransactionHistory(const int64 ChainID, const FSeqGetTransactionHistoryArgs& Args, TSuccessCallback<FSeqGetTransactionHistoryReturn> OnSuccess, const FFailureCallback& OnFailure)
{
	HTTPPost(ChainID, "GetTransactionHistory", BuildArgs<FSeqGetTransactionHistoryArgs>(Args), [this,OnSuccess](const FString& Content)
	{
		OnSuccess(this->BuildResponse<FSeqGetTransactionHistoryReturn>(Content));
	}, OnFailure);
}

TMap<int64, FSeqTokenBalance> UIndexer::GetTokenBalancesAsMap(TArray<FSeqTokenBalance> Balances)
{
	TMap<int64, FSeqTokenBalance> BalanceMap;

	for (FSeqTokenBalance Balance : Balances)
	{
		BalanceMap.Add(TPair<int64,FSeqTokenBalance>(Balance.tokenID,Balance));
	}
	
	return BalanceMap;
}