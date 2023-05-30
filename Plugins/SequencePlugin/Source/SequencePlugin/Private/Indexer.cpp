// Fill out your copyright notice in the Description page of Project Settings.


#include "Indexer.h"
#include "JsonBuilder.h"

UIndexer::UIndexer()
{
	this->Indexernames.Add(TPair<int64, FString>(1337, "testchain"));//there are others listed but we don't have them yet
}

/*
	Combines <see cref="PATH" and name="name" to suffix on to the base address
*/
FString UIndexer::Url(int64 chainID, FString endPoint)
{
	FString out_url = HostName(chainID);
	out_url.Append(this->PATH);
	out_url.Append(endPoint);
	return out_url;
}


/*
	Get hostname directing to specific chainID
*/
FString UIndexer::HostName(int64 chainID)
{
	FString hostname = "https://";
	hostname.Append(*this->Indexernames.Find(chainID));
	hostname.Append("-indexer.sequence.app");
	return hostname;
}

/*
	Here we construct a post request and parse out a response if valid.
*/
FString UIndexer::HTTPPost(int64 chainID, FString endpoint, FString args)
{
	FString response = "[NO RESPONSE]";
	//Now we create the post request
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("POST");
	http_post_req->SetHeader("Content-Type", "application/json");//2 differing headers for the request
	http_post_req->SetHeader("Accept", "application/json");
	http_post_req->SetURL(this->Url(chainID, endpoint));
	http_post_req->SetContentAsString(args);//args will need to be a json object converted to a string

	if (http_post_req->ProcessRequest())//hoping this is a blocking call alternatively I can use the delegates to make this async!
	{//success
		for (uint8 i : http_post_req->GetContent())//Here I process the response 1 byte at a time!
		{
			const uint8 * i_ptr = &i;
			FString data_byte = BytesToString(i_ptr,1);
			response.Append(data_byte);
		}
	}
	else
	{//failed
		UE_LOG(LogTemp, Warning, TEXT("[Error Parsing response from Sequence Server]"));
		response = "[Error Response]";
	}
	return response;
}

void UIndexer::Remove_Json_SNRT_INLINE(FString * json_string_in)
{

	(*json_string_in).RemoveSpacesInline();

	FString srch_n = TEXT("\n");
	FString srch_r = TEXT("\r");
	FString srch_t = TEXT("\t");
	FString replace = TEXT("");

	const TCHAR* rep_ptr = *replace;
	const TCHAR* srch_ptr_n = *srch_n;
	const TCHAR* srch_ptr_r = *srch_r;
	const TCHAR* srch_ptr_t = *srch_t;

	(*json_string_in).ReplaceInline(srch_ptr_n, rep_ptr, ESearchCase::IgnoreCase);//remove \n
	(*json_string_in).ReplaceInline(srch_ptr_r, rep_ptr, ESearchCase::IgnoreCase);//remove \r
	(*json_string_in).ReplaceInline(srch_ptr_t, rep_ptr, ESearchCase::IgnoreCase);//remove \t
}

/*
	Here we take in a struct and convert it straight into a json object String
	@Param (T) Struct_in the struct we are converting to a json object string
	@Return the JSON Object String
*/
template < typename T> FString UIndexer::BuildArgs(T struct_in)
{
	FString result = "[FAILED TO PARSE]";
	//There are 3 special cases for this function all 3 involve nested data structures,
	//since unreal doesn't support nested TArray's and TMap's we need to look out for this nesting and handle it seperate from here!
	//calls for parsing
	if (typeid(T) == typeid(FGetTokenSuppliesMapReturn))
	{

	}
	else if (typeid(T) == typeid(FGetTokenSuppliesMapArgs))
	{

	}
	else if (typeid(T) == typeid(FTokenMetaData))
	{

	}
	else//handle all othercases like normal as they make use of primitives or other ustructs
	{
		try
		{
			if (!FJsonObjectConverter::UStructToJsonObjectString<T>(struct_in, result))
			{
				UE_LOG(LogTemp, Display, TEXT("Failed to convert specified UStruct to a json object\n"));
			}
		}
		catch (...)
		{
			UE_LOG(LogTemp, Display, TEXT("Exception thrown trying to create FString from UStruct\n"));
		}
	}
	return result;
}

//generic
template<typename T> T UIndexer::BuildResponse(FString text)
{
	//Take the FString and convert it to a JSON object first!
	TSharedPtr<FJsonObject> json_step;
	//Then take the json object we make and convert it to a USTRUCT of type T then we return that!
	T ret_struct;

	try//first we create the json_object as we need this in all cases
	{
		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(text), json_step))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *text);
		}
	}
	catch (...)
	{
		UE_LOG(LogTemp, Display, TEXT("Exception thrown trying to create json object from text\n"));
	}

	try
	{
		if (!FJsonObjectConverter::JsonObjectToUStruct<T>(json_step.ToSharedRef(), &ret_struct))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert Json Object: %s to USTRUCT of type T"), *text);
		}
	}
	catch (...)
	{
		UE_LOG(LogTemp, Display, TEXT("Exception thrown trying to create UStruct from Json object\n"));
	}

	return ret_struct;
}

//specific start

template<> FStruct_0 UIndexer::BuildResponse(FString text)
{
	TSharedPtr<FJsonObject> json_step;

	try//first we create the json_object as we need this in all cases
	{
		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(text), json_step))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *text);
		}
	}
	catch (...)
	{
		UE_LOG(LogTemp, Display, TEXT("Exception thrown trying to create json object from text\n"));
	}
	FStruct_0 data(*json_step.Get());//use our custom constructor instead!
	return data;
}

template<> FGetTokenSuppliesMapReturn UIndexer::BuildResponse(FString text)
{
	TSharedPtr<FJsonObject> json_step;

	try//first we create the json_object as we need this in all cases
	{
		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(text), json_step))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *text);
		}
	}
	catch (...)
	{
		UE_LOG(LogTemp, Display, TEXT("Exception thrown trying to create json object from text\n"));
	}
	FGetTokenSuppliesMapReturn data (*json_step.Get());//use our custom constructor instead!
	return data;
}

template<> FGetTokenSuppliesMapArgs UIndexer::BuildResponse(FString text)
{
	TSharedPtr<FJsonObject> json_step;

	try//first we create the json_object as we need this in all cases
	{
		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(text), json_step))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *text);
		}
	}
	catch (...)
	{
		UE_LOG(LogTemp, Display, TEXT("Exception thrown trying to create json object from text\n"));
	}
	FGetTokenSuppliesMapArgs data(*json_step.Get());//use our custom constructor instead!
	return data;
}

template<> FTokenMetaData UIndexer::BuildResponse(FString text)
{
	TSharedPtr<FJsonObject> json_step;

	try//first we create the json_object as we need this in all cases
	{
		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(text), json_step))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *text);
		}
	}
	catch (...)
	{
		UE_LOG(LogTemp, Display, TEXT("Exception thrown trying to create json object from text\n"));
	}
	FTokenMetaData data(*json_step.Get());//use our custom constructor instead!
	return data;
}

//specific end

template <typename T> bool UIndexer::Test_Json_Parsing(FString json_in, FString type)
{
	UE_LOG(LogTemp, Display, TEXT("====================================================================="));
	UE_LOG(LogTemp, Display, TEXT("JSON Type %s"), *type);

	T built_response = BuildResponse<T>(json_in);

	FString out;
	FJsonObjectConverter::UStructToJsonObjectString<T>(built_response, out);

	FString* o_ptr = &out;
	this->Remove_Json_SNRT_INLINE(o_ptr);//removes spaces, /n, /r, /t
	FString* in_ptr = &json_in;
	this->Remove_Json_SNRT_INLINE(in_ptr);

	UE_LOG(LogTemp, Display, TEXT("JSON_in %s"), *json_in);
	UE_LOG(LogTemp, Display, TEXT("resulting jsonString: %s"), *out);

	bool result = (out.ToLower().Compare(json_in.ToLower()) == 0);

	if (result)
	{
		UE_LOG(LogTemp, Display, TEXT("Test passed\n"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Test failed\n"));
	}
	UE_LOG(LogTemp, Display, TEXT("====================================================================="));
	return result;
}

void UIndexer::testing()
{
	bool res = true;
	//FString json_0 = "{\"list\":[[0,1,2],[3,4,5],[6,7,8]]}";
	FString json_0 = "{ \"map\": { \"key1\":[0,1,2] , \"key2\":[3,4,5] , \"key3\":[6,7,8] } }";
	res &= Test_Json_Parsing<FStruct_0>(json_0, "FStruct_0");
	if (res)
	{
		UE_LOG(LogTemp, Display, TEXT("\n[Tests Passed]\n"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("\n[Tests failed]\n"));
	}
}

bool UIndexer::Ping(int64 chainID)
{
	FPingReturn response = BuildResponse<FPingReturn>(HTTPPost(chainID,"Ping",""));
	return response.status;
}

FVersion UIndexer::Version(int64 chainID)
{
	FVersionReturn response = BuildResponse<FVersionReturn>(HTTPPost(chainID, "Version", ""));
	return response.version;
}

//need to assess task structure
FRuntimeStatus UIndexer::RunTimeStatus(int64 chainID)
{
	FRuntimeStatusReturn response = BuildResponse<FRuntimeStatusReturn>(HTTPPost(chainID,"RuntimeStatus",""));
	return response.status;
}

int64 UIndexer::GetChainID(int64 chainID)
{
	FGetChainIDReturn response = BuildResponse<FGetChainIDReturn>(HTTPPost(chainID, "GetChainID",""));
	return response.chainID;
}

FEtherBalance UIndexer::GetEtherBalance(int64 chainID, FString accountAddr)
{
	FGetEtherBalanceReturn response = BuildResponse<FGetEtherBalanceReturn>(HTTPPost(chainID, "GetEtherBalance", accountAddr));
	return response.balance;
}

//args should be of type FGetTokenBalancesArgs we need to parse these things down to json strings!
FGetTokenBalancesReturn UIndexer::GetTokenBalances(int64 chainID, FGetTokenBalancesArgs args)
{
	return BuildResponse<FGetTokenBalancesReturn>(HTTPPost(chainID, "GetTokenBalances", BuildArgs<FGetTokenBalancesArgs>(args)));
}

FGetTokenSuppliesReturn UIndexer::GetTokenSupplies(int64 chainID, FGetTokenSuppliesArgs args)
{
	return BuildResponse<FGetTokenSuppliesReturn>(HTTPPost(chainID,"GetTokenSupplies", BuildArgs<FGetTokenSuppliesArgs>(args)));
}

FGetTokenSuppliesMapReturn UIndexer::GetTokenSuppliesMap(int64 chainID, FGetTokenSuppliesMapArgs args)
{
	return BuildResponse<FGetTokenSuppliesMapReturn>(HTTPPost(chainID,"GetTokenSuppliesMap", BuildArgs<FGetTokenSuppliesMapArgs>(args)));
}

FGetBalanceUpdatesReturn UIndexer::GetBalanceUpdates(int64 chainID, FGetBalanceUpdatesArgs args)
{
	return BuildResponse<FGetBalanceUpdatesReturn>(HTTPPost(chainID,"GetBalanceUpdates", BuildArgs<FGetBalanceUpdatesArgs>(args)));
}

FGetTransactionHistoryReturn UIndexer::GetTransactionHistory(int64 chainID, FGetTransactionHistoryArgs args)
{
	return BuildResponse<FGetTransactionHistoryReturn>(HTTPPost(chainID,"GetTransactionHistory", BuildArgs<FGetTransactionHistoryArgs>(args)));
}