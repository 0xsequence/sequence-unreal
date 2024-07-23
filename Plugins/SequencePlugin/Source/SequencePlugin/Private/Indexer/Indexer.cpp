// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.

#include "Indexer/Indexer.h"
#include "Util/Async.h"
#include "JsonObjectConverter.h"
#include "Http.h"
#include "HttpManager.h"

const TMap<int64, FString> UIndexer::IndexerNames = {{1,"mainnet"},{137,"polygon"},{1101,"polygon-zkevm"},{42161,"arbitrum"},{42170,"arbitrum-nova"},{10,"optimism"},{56,"bsc"},{43114,"avalanche"},{8453,"base"},{100,"gnosis"},{1337,"testchain"},{5,"goerli"},{11155111,"sepolia"},{421613,"arbitrum-goerli"},{80001,"mumbai"},{84531,"base-goerli"},{97,"bsc-testnet"},{43113,"testnetavalanchefuji"} };

UIndexer::UIndexer()
{
}

TMap<int64, FString> UIndexer::GetIndexerNames()
{	
	return IndexerNames;
}

FString UIndexer::GetIndexerName(int64 chainId)
{
	FString ret = "[ERROR][Name not found]";

	if (IndexerNames.Contains(chainId))
	{
		ret = *IndexerNames.Find(chainId);
	}

	return ret;
}

/*
	Combines <see cref="PATH" and name="name" to suffix on to the base address
*/
FString UIndexer::Url(const int64& ChainID,const FString& endPoint)
{
	FString out_url = HostName(ChainID);
	out_url.Append(this->PATH);
	

	if (endPoint.Len() > 0)
	{
		out_url.Append(endPoint);
	}
	return out_url;
}

/*
	Get hostname directing to specific chainID
*/
FString UIndexer::HostName(int64 ChainID)
{
	FString hostname = "https://";
	hostname.Append(*GetIndexerNames().Find(ChainID));
	hostname.Append("-indexer.sequence.app");
	return hostname;
}

/*
	Here we construct a post request and parse out a response if valid.
*/
void UIndexer::HTTPPost(const int64& ChainID,const FString& Endpoint,const FString& Args, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	const TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	
	http_post_req->SetVerb("POST");
	http_post_req->SetHeader("Content-Type", "application/json");//2 differing headers for the request
	http_post_req->SetHeader("Accept", "application/json");
	http_post_req->SetTimeout(30);
	http_post_req->SetURL(this->Url(ChainID, Endpoint));
	http_post_req->SetContentAsString(Args);//args will need to be a json object converted to a string
	
	http_post_req->OnProcessRequestComplete().BindLambda([=](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{ 
		if(bWasSuccessful )
		{
			const FString Content = Request->GetResponse()->GetContentAsString();
			OnSuccess(Content);
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

/*
	Here we take in a struct and convert it straight into a json object String
	@Param (T) Struct_in the struct we are converting to a json object string
	@Return the JSON Object String
*/
template < typename T> FString UIndexer::BuildArgs(T StructIn)
{
	FString result = "[FAILED TO PARSE]";
	if (StructIn.customGetter)
	{
		result = StructIn.GetArgs();
	}
	else
	{
		if (!FJsonObjectConverter::UStructToJsonObjectString<T>(StructIn, result))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert specified UStruct to a json object\n"));
		}
	}
	return result;
}

//generic
template<typename T> T UIndexer::BuildResponse(FString Text)
{
	//Take the FString and convert it to a JSON object first!
	TSharedPtr<FJsonObject> json_step;
	//Then take the json object we make and convert it to a USTRUCT of type T then we return that!
	T ret_struct;

		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), json_step))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *Text);
			return T();
		}
		//this next line with throw an exception in null is used as an entry in json attributes! we need to remove null entries
		if (ret_struct.customConstructor) 
		{//use the custom constructor!
			ret_struct.construct(*json_step.Get());
		}
		else
		{//use unreal parsing!
			if (!FJsonObjectConverter::JsonObjectToUStruct<T>(json_step.ToSharedRef(), &ret_struct))
			{
				UE_LOG(LogTemp, Display, TEXT("Failed to convert Json Object: %s to USTRUCT of type T"), *Text);
				return T();
			}
		}
		ret_struct.Setup(*json_step.Get());//now for the edge cases we will manually inject the data where it needs to be!

	return ret_struct;
}

void UIndexer::Ping(int64 ChainID, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(ChainID, "Ping", "", [this,OnSuccess](FString Content) {
		OnSuccess(this->BuildResponse<FPingReturn>(Content).status);
	}, OnFailure);
}

void UIndexer::Version(int64 ChainID, TSuccessCallback<FVersion> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(ChainID, "Version", "", [this,OnSuccess](FString Content)
	{
		OnSuccess(this->BuildResponse<FVersionReturn>(Content).version);
	}, OnFailure);
}

void UIndexer::RunTimeStatus(int64 ChainID, TSuccessCallback<FRuntimeStatus> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(ChainID, "RuntimeStatus", "", [this,OnSuccess](FString Content)
	{
		OnSuccess(this->BuildResponse<FRuntimeStatusReturn>(Content).status);
	}, OnFailure);
}

void UIndexer::GetChainID(int64 ChainID, TSuccessCallback<int64> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(ChainID, "GetChainID", "", [this,OnSuccess](FString Content)
	{
		OnSuccess(this->BuildResponse<FGetChainIDReturn>(Content).chainID);
	}, OnFailure);
}

void UIndexer::GetEtherBalance(int64 ChainID, FString AccountAddr, TSuccessCallback<FEtherBalance> OnSuccess, FFailureCallback OnFailure)
{//since we are given a raw accountAddress we compose the json arguments here to put in the request manually
	FString json_arg = "{\"accountAddress\":\"";
	json_arg.Append(AccountAddr);
	json_arg.Append("\"}");

	HTTPPost(ChainID, "GetEtherBalance", json_arg, [this,OnSuccess](FString Content)
	{
		FGetEtherBalanceReturn Response = this->BuildResponse<FGetEtherBalanceReturn>(Content);
		OnSuccess(Response.balance);
	}, OnFailure);
}

void UIndexer::GetTokenBalances(int64 ChainID, FGetTokenBalancesArgs Args, TSuccessCallback<FGetTokenBalancesReturn> OnSuccess, FFailureCallback OnFailure)
{
	const FString endpoint = "GetTokenBalances";
	HTTPPost(ChainID, endpoint, BuildArgs<FGetTokenBalancesArgs>(Args), [this,OnSuccess](FString Content)
	{
		UE_LOG(LogTemp,Display,TEXT("%s"),*Content);
		FGetTokenBalancesReturn response = this->BuildResponse<FGetTokenBalancesReturn>(Content);
		OnSuccess(response);
	}, OnFailure);
}

void UIndexer::GetTokenSupplies(int64 ChainID, FGetTokenSuppliesArgs Args, TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(ChainID, "GetTokenSupplies", BuildArgs<FGetTokenSuppliesArgs>(Args), [this,OnSuccess](FString Content)
	{
		OnSuccess(this->BuildResponse<FGetTokenSuppliesReturn>(Content));
	}, OnFailure);
}

void UIndexer::GetTokenSuppliesMap(int64 ChainID, FGetTokenSuppliesMapArgs Args, TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(ChainID, "GetTokenSuppliesMap", BuildArgs<FGetTokenSuppliesMapArgs>(Args), [this,OnSuccess](FString Content)
	{
		OnSuccess(this->BuildResponse<FGetTokenSuppliesMapReturn>(Content));
	}, OnFailure);
}

void UIndexer::GetBalanceUpdates(int64 ChainID, FGetBalanceUpdatesArgs Args, TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(ChainID, "GetBalanceUpdates", BuildArgs<FGetBalanceUpdatesArgs>(Args), [this,OnSuccess](FString Content)
	{
		OnSuccess(this->BuildResponse<FGetBalanceUpdatesReturn>(Content));
	}, OnFailure);
}

void UIndexer::GetTransactionHistory(int64 ChainID, FGetTransactionHistoryArgs Args, TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(ChainID, "GetTransactionHistory", BuildArgs<FGetTransactionHistoryArgs>(Args), [this,OnSuccess](FString Content)
	{
		OnSuccess(this->BuildResponse<FGetTransactionHistoryReturn>(Content));
	}, OnFailure);
}

TMap<int64, FTokenBalance> UIndexer::GetTokenBalancesAsMap(TArray<FTokenBalance> Balances)
{
	TMap<int64, FTokenBalance> BalanceMap;

	for (FTokenBalance Balance : Balances)
	{
		BalanceMap.Add(TPair<int64,FTokenBalance>(Balance.tokenID,Balance));
	}
	
	return BalanceMap;
}