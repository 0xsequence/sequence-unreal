// Fill out your copyright notice in the Description page of Project Settings.


#include "Indexer/Indexer.h"
#include "Util/Async.h"

UIndexer::UIndexer()
{
}

TMap<int64, FString> UIndexer::GetIndexerNames()
{
	TMap<int64, FString> IndexerNames;
	//Mainnets
	IndexerNames.Add(1, "mainnet");
	IndexerNames.Add(137, "polygon");
	IndexerNames.Add(1101, "polygon-zkevm");
	IndexerNames.Add(42161, "arbitrum");
	IndexerNames.Add(42170, "arbitrum-nova");
	IndexerNames.Add(10, "optimism");
	IndexerNames.Add(56, "bsc");
	IndexerNames.Add(43114, "avalanche");
	IndexerNames.Add(8453, "base");
	IndexerNames.Add(100, "gnosis");

	//Testnets
	IndexerNames.Add(1337, "testchain");
	IndexerNames.Add(5, "goerli");
	IndexerNames.Add(11155111, "sepolia");
	IndexerNames.Add(421613, "arbitrum-goerli");
	IndexerNames.Add(80001, "mumbai");
	IndexerNames.Add(84531, "base-goerli");
	IndexerNames.Add(97, "bsc-testnet");
	IndexerNames.Add(43113, "testnetavalanchefuji");
	
	return IndexerNames;
}

FString UIndexer::GetIndexerName(int64 chainId)
{
	TMap<int64, FString> IndexerNames;

	//Mainnets
	IndexerNames.Add(1, "mainnet");
	IndexerNames.Add(137, "polygon");
	IndexerNames.Add(1101, "polygon-zkevm");
	IndexerNames.Add(42161, "arbitrum");
	IndexerNames.Add(42170, "arbitrum-nova");
	IndexerNames.Add(10, "optimism");
	IndexerNames.Add(56, "bsc");
	IndexerNames.Add(43114, "avalanche");
	IndexerNames.Add(8453, "base");
	IndexerNames.Add(100, "gnosis");

	//Testnets
	IndexerNames.Add(1337, "testchain");
	IndexerNames.Add(5, "goerli");
	IndexerNames.Add(11155111, "sepolia");
	IndexerNames.Add(421613,"arbitrum-goerli");
	IndexerNames.Add(80001, "mumbai");
	IndexerNames.Add(84531, "base-goerli");
	IndexerNames.Add(97, "bsc-testnet");
	IndexerNames.Add(43113, "testnetavalanchefuji");

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
FString UIndexer::Url(const int64& chainID,const FString& endPoint)
{
	FString out_url = HostName(chainID);
	out_url.Append(this->PATH);
	

	if (endPoint.Len() > 0)
	{//strange error cropping up from this
		out_url.Append(endPoint);
	}
	return out_url;
}

/*
	Get hostname directing to specific chainID
*/
FString UIndexer::HostName(int64 chainID)
{
	FString hostname = "https://";
	hostname.Append(*GetIndexerNames().Find(chainID));
	hostname.Append("-indexer.sequence.app");
	return hostname;
}

/*
	Here we construct a post request and parse out a response if valid.
*/
void UIndexer::HTTPPost(const int64& chainID,const FString& endpoint,const FString& args, TSuccessCallback<FString> OnSuccess, FFailureCallback OnFailure)
{
	//Now we create the post request
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();

	http_post_req->SetVerb("POST");
	http_post_req->SetHeader("Content-Type", "application/json");//2 differing headers for the request
	http_post_req->SetHeader("Accept", "application/json");
	http_post_req->SetURL(this->Url(chainID, endpoint));
	http_post_req->SetContentAsString(args);//args will need to be a json object converted to a string

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
template < typename T> FString UIndexer::BuildArgs(T struct_in)
{
	FString result = "[FAILED TO PARSE]";
	if (struct_in.customGetter)
	{
		result = struct_in.GetArgs();
	}
	else
	{
		if (!FJsonObjectConverter::UStructToJsonObjectString<T>(struct_in, result))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert specified UStruct to a json object\n"));
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Args: %s"),*result);
	return result;
}

//generic
template<typename T> T UIndexer::BuildResponse(FString text)
{
	UE_LOG(LogTemp, Display, TEXT("Rep:\n%s"), *text);
	//Take the FString and convert it to a JSON object first!
	TSharedPtr<FJsonObject> json_step;
	//Then take the json object we make and convert it to a USTRUCT of type T then we return that!
	T ret_struct;

		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(text), json_step))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *text);
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
				UE_LOG(LogTemp, Display, TEXT("Failed to convert Json Object: %s to USTRUCT of type T"), *text);
				return T();
			}
		}
		ret_struct.setup(*json_step.Get());//now for the edge cases we will manually inject the data where it needs to be!

	return ret_struct;
}

void UIndexer::Ping(int64 chainID, TSuccessCallback<bool> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(chainID, "Ping", "", [=](FString Content) {
		OnSuccess(BuildResponse<FPingReturn>(Content).status);
	}, OnFailure);
}

void UIndexer::Version(int64 chainID, TSuccessCallback<FVersion> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(chainID, "Version", "", [=](FString Content)
	{
		OnSuccess(BuildResponse<FVersionReturn>(Content).version);
	}, OnFailure);
}

void UIndexer::RunTimeStatus(int64 chainID, TSuccessCallback<FRuntimeStatus> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(chainID, "RuntimeStatus", "", [=](FString Content)
	{
		OnSuccess(BuildResponse<FRuntimeStatusReturn>(Content).status);
	}, OnFailure);
}

void UIndexer::GetChainID(int64 chainID, TSuccessCallback<int64> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(chainID, "GetChainID", "", [=](FString Content)
	{
		OnSuccess(BuildResponse<FGetChainIDReturn>(Content).chainID);
	}, OnFailure);
}

void UIndexer::GetEtherBalance(int64 chainID, FString accountAddr, TSuccessCallback<FEtherBalance> OnSuccess, FFailureCallback OnFailure)
{//since we are given a raw accountAddress we compose the json arguments here to put in the request manually
	FString json_arg = "{\"accountAddress\":\"";
	json_arg.Append(accountAddr);
	json_arg.Append("\"}");

	HTTPPost(chainID, "GetEtherBalance", json_arg, [=](FString Content)
	{
		FGetEtherBalanceReturn Response = BuildResponse<FGetEtherBalanceReturn>(Content);
		OnSuccess(Response.balance);
	}, OnFailure);
}

//args should be of type FGetTokenBalancesArgs we need to parse these things down to json strings!
void UIndexer::GetTokenBalances(int64 chainID, FGetTokenBalancesArgs args, TSuccessCallback<FGetTokenBalancesReturn> OnSuccess, FFailureCallback OnFailure)
{
	const FString endpoint = "GetTokenBalances";
	UE_LOG(LogTemp, Display, TEXT("Endpoint: [%s]"), *endpoint);
	HTTPPost(chainID, endpoint, BuildArgs<FGetTokenBalancesArgs>(args), [=](FString Content)
	{
		FGetTokenBalancesReturn response = BuildResponse<FGetTokenBalancesReturn>(Content);
		OnSuccess(response);
	}, OnFailure);
}

void UIndexer::GetTokenSupplies(int64 chainID, FGetTokenSuppliesArgs args, TSuccessCallback<FGetTokenSuppliesReturn> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(chainID, "GetTokenSupplies", BuildArgs<FGetTokenSuppliesArgs>(args), [=](FString Content)
	{
		OnSuccess(BuildResponse<FGetTokenSuppliesReturn>(Content));
	}, OnFailure);
}

void UIndexer::GetTokenSuppliesMap(int64 chainID, FGetTokenSuppliesMapArgs args, TSuccessCallback<FGetTokenSuppliesMapReturn> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(chainID, "GetTokenSuppliesMap", BuildArgs<FGetTokenSuppliesMapArgs>(args), [=](FString Content)
	{
		OnSuccess(BuildResponse<FGetTokenSuppliesMapReturn>(Content));
	}, OnFailure);
}

void UIndexer::GetBalanceUpdates(int64 chainID, FGetBalanceUpdatesArgs args, TSuccessCallback<FGetBalanceUpdatesReturn> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(chainID, "GetBalanceUpdates", BuildArgs<FGetBalanceUpdatesArgs>(args), [=](FString Content)
	{
		OnSuccess(BuildResponse<FGetBalanceUpdatesReturn>(Content));
	}, OnFailure);
}

void UIndexer::GetTransactionHistory(int64 chainID, FGetTransactionHistoryArgs args, TSuccessCallback<FGetTransactionHistoryReturn> OnSuccess, FFailureCallback OnFailure)
{
	HTTPPost(chainID, "GetTransactionHistory", BuildArgs<FGetTransactionHistoryArgs>(args), [=](FString Content)
	{
		OnSuccess(BuildResponse<FGetTransactionHistoryReturn>(Content));
	}, OnFailure);
}