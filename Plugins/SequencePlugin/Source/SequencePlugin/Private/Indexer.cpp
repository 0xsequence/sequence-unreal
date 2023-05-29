// Fill out your copyright notice in the Description page of Project Settings.


#include "Indexer.h"
#include "JsonBuilder.h"

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
	//Now we create the post request
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("POST");
	http_post_req->SetHeader("Content-Type", "application/json");//2 differing headers for the request
	http_post_req->SetHeader("Accept", "application/json");
	http_post_req->SetURL(this->Url(chainID, endpoint));
	http_post_req->SetContentAsString(args);//args will need to be a json object converted to a string

	if (http_post_req->ProcessRequest())//hoping this is a blocking call alternatively I can use the delegates to make this async!
	{//success
		http_post_req->GetResponse()->GetContentType();
		//this is where we can process the response!
	}
	else
	{//failed
	}
	return FString();
}

template<typename T> T UIndexer::BuildResponse(FString text)
{
	//Take the FString and convert it to a JSON object first!
	TSharedPtr<FJsonObject> json_step;
	//Then take the json object we make and convert it to a USTRUCT of type T then we return that!
	T ret_struct;

	//There are 3 special cases for this function all 3 involve nested data structures,
	//since unreal doesn't support nested TArray's and TMap's we need to look at out for this nested
	//calls for parsing

	//try Try catch is no longer working for reasons?
	//{
		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(text), json_step))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *text);
		}
	//}
	//catch (...)
	//{
		UE_LOG(LogTemp, Display, TEXT("Exception thrown trying to create json object from text\n"));
	//}

	//try
	//{
		if (!FJsonObjectConverter::JsonObjectToUStruct<T>(json_step.ToSharedRef(), &ret_struct))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert Json Object: %s to USTRUCT of type T"), *text);
		}
	//}
	//catch (...)
	//{
		UE_LOG(LogTemp, Display, TEXT("Exception thrown trying to create UStruct from Json object\n"));
	//}

	return ret_struct;
}

template <typename T> bool UIndexer::Test_Func(FString json_in, FString type)
{
	UE_LOG(LogTemp, Display, TEXT("====================================================================="));
	UE_LOG(LogTemp, Display, TEXT("JSON_in %s"), *json_in);
	UE_LOG(LogTemp, Display, TEXT("JSON Type %s"), *type);
	T built_response = BuildResponse<T>(json_in);
	FString out;
	FJsonObjectConverter::UStructToJsonObjectString<T>(built_response,out);
	out.RemoveSpacesInline();
	TArray<TCHAR>* c_ptr;
	TArray<TCHAR> Chars;
	Chars.Add('\r');
	Chars.Add('\n\r');
	Chars.Add('\r\n');
	Chars.Add('\t');
	Chars.Add('\n');
	Chars.Add('\t\t');
	c_ptr = &Chars;

	out.ReplaceEscapedCharWithCharInline();//this should be removing ALL /* but for some reason isn't doing it?

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
	FString json_0 = "{\n\t\"age\":69,\n\t\"name\":\"struct_0_JSON\"\n}";
	FString json_1 = "{\"age\":96,\"name\":\"struct_1_JSON\"}";

	bool var = Test_Func<FStruct_0>(json_0, "FStruct_0");

	//FStruct_0 s_0 = BuildResponse<FStruct_0>(json_0);//generic json parsing!
	//FStruct_1 s_1 = BuildResponse<FStruct_1>(json_1);

	//UE_LOG(LogTemp, Display, TEXT("JSON_0 in %s\n"), *json_0);
	//UE_LOG(LogTemp, Display, TEXT("S_0 age: %d"), s_0.age);
	//UE_LOG(LogTemp, Display, TEXT("S_0 name: %s"), *s_0.name);

	//UE_LOG(LogTemp, Display, TEXT("JSON_1 in %s\n"), *json_1);
	//UE_LOG(LogTemp, Display, TEXT("S_1 age: %d"), s_1.age);
	//UE_LOG(LogTemp, Display, TEXT("S_1 name: %s"), *s_1.name);
}

UIndexer::UIndexer()
{
	this->Indexernames.Add(TPair<int64,FString>(1337,"testchain"));//there are others listed but we don't have them yet
}

