// Fill out your copyright notice in the Description page of Project Settings.


#include "Indexer.h"

#include "Async.h"
#include "JsonBuilder.h"

UIndexer::UIndexer()
{
	this->Indexernames.Add(TPair<int64, FString>(1337, "testchain"));//there are others listed but we don't have them yet
	this->Indexernames.Add(TPair<int64, FString>(137,"polygon"));//need to verify that this is polygons chain id!
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
TFuture<FString> UIndexer::HTTPPost(int64 chainID, FString endpoint, FString args)
{
	
	//Now we create the post request
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();

	http_post_req->SetVerb("POST");
	http_post_req->SetHeader("Content-Type", "application/json");//2 differing headers for the request
	http_post_req->SetHeader("Accept", "application/json");
	http_post_req->SetURL(this->Url(chainID, endpoint));
	http_post_req->SetContentAsString(args);//args will need to be a json object converted to a string
	http_post_req->ProcessRequest();

	return Async(EAsyncExecution::Thread, [http_post_req, this, args]()
	{
		double LastTime = FPlatformTime::Seconds();
		while (EHttpRequestStatus::Processing == http_post_req->GetStatus())
		{
			const double AppTime = FPlatformTime::Seconds();
			FHttpModule::Get().GetHttpManager().Tick(AppTime - LastTime);
			LastTime = AppTime;
			FPlatformProcess::Sleep(0.5f);
		}

		auto response = http_post_req.Get().GetResponse().Get()->GetContentAsString();

		UE_LOG(LogTemp, Display, TEXT("Args in: %s"), *args);

		UE_LOG(LogTemp, Display, TEXT("Response: %s"),*response);
		return response;
	});
}

void UIndexer::Remove_Json_SNRT_INLINE(FString* json_string_in)
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
	if (struct_in.customGetter)
	{
		result = struct_in.Get();
	}
	else
	{
		if (!FJsonObjectConverter::UStructToJsonObjectString<T>(struct_in, result))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert specified UStruct to a json object\n"));
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

		if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(text), json_step))
		{
			UE_LOG(LogTemp, Display, TEXT("Failed to convert String: %s to Json object"), *text);
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
			}
		}
		ret_struct.setup(*json_step.Get());//now for the edge cases we will manually inject the data where it needs to be!

	return ret_struct;
}

//0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9 peter's public addr
template <typename T> bool UIndexer::Test_Json_Parsing(FString json_in, FString type)
{
	UE_LOG(LogTemp, Display, TEXT("====================================================================="));
	UE_LOG(LogTemp, Display, TEXT("JSON Type %s"), *type);

	T built_response = BuildResponse<T>(json_in);
	FString intermediate_out;
	FJsonObjectConverter::UStructToJsonObjectString<T>(built_response, intermediate_out);

	FString testable_out = BuildArgs<T>(built_response);

	FString* i_ptr = &intermediate_out;
	this->Remove_Json_SNRT_INLINE(i_ptr);
	FString* o_ptr = &testable_out;
	this->Remove_Json_SNRT_INLINE(o_ptr);//removes spaces, /n, /r, /t
	FString* in_ptr = &json_in;
	this->Remove_Json_SNRT_INLINE(in_ptr);

	UE_LOG(LogTemp, Display, TEXT("JSON_in %s"), *json_in);

	UE_LOG(LogTemp, Display, TEXT("Intermediate Json %s"), *intermediate_out);

	UE_LOG(LogTemp, Display, TEXT("resulting jsonString: %s"), *testable_out);

	bool result = (testable_out.ToLower().Compare(json_in.ToLower()) == 0);

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

void UIndexer::setup(ASequence_Backend_Manager* manager_ref)
{
	this->bck_mngr = manager_ref;
}

TArray<UTexture2D*> UIndexer::testing()
{
	bool res = true;
	TArray<UTexture2D*> ret;
	FGetTransactionHistoryArgs args;// ("0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9");
	FTransactionHistoryFilter filter;

	FGetTokenBalancesArgs t_args;
	t_args.accountAddress = "0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9";

	filter.accountAddress = "0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9";
	args.filter = filter;
	args.includeMetaData = true;

	FGetTransactionHistoryReturn data = GetTransactionHistory(137,args);
	UE_LOG(LogTemp, Display, TEXT("Done testing history\n"));

	GetEtherBalance(137, "0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9");
	UE_LOG(LogTemp, Display, TEXT("Done testing eth balance\n"));

	FGetTokenBalancesArgs bln_args;
	bln_args.accountAddress = "0x8e3E38fe7367dd3b52D1e281E4e8400447C8d8B9";
	FGetTokenBalancesReturn bln_ret = GetTokenBalances(137, bln_args);
	UE_LOG(LogTemp, Display, TEXT("Done testing get token balances\n"));


	/*
	UTexture2D* fetched_img = NULL;
	for (auto i : data.transactions)//all txn's
	{
		for (auto j : i.transfers)//goes through transfers of a transaction!
		{
			fetched_img = get_image_data(j.contractInfo.extensions.ogImage);
			if(fetched_img != NULL)
				ret.Add(fetched_img);//this will hammer the call to see how it will behave
			TArray<FString> keys;
			j.tokenMetaData.GetKeys(keys);
			for (auto key : keys)//the metadata of a transfer!
			{
				auto data_e = j.tokenMetaData.Find(key);
				fetched_img = get_image_data(data_e->image);
				if (fetched_img != NULL)
					ret.Add(fetched_img);//this will hammer the call to see how it will behave
			}
		}
	}
	*/
	UE_LOG(LogTemp, Display, TEXT("Img amount %d"),ret.Num());

	return ret;
}



UTexture2D* UIndexer::get_image_data(FString URL)
{
	UE_LOG(LogTemp, Display, TEXT("Img from URL: %s"),*URL);
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	UTexture2D * ret = NULL;
	http_post_req->SetVerb("GET");
	http_post_req->SetURL(URL);
	http_post_req->ProcessRequest();

	//going forward these calls must be made ASYNC otherwise we will be constantly stalling at the start for
	//very long periods of time look at setting up a dynamic approach to solve this properly
	double LastTime = FPlatformTime::Seconds();
	while (EHttpRequestStatus::Processing == http_post_req->GetStatus())
	{
		const double AppTime = FPlatformTime::Seconds();
		FHttpModule::Get().GetHttpManager().Tick(AppTime - LastTime);
		LastTime = AppTime;
		FPlatformProcess::Sleep(0.5f);
	}

	if (http_post_req.Get().GetResponse()) 
	{
		UE_LOG(LogTemp, Display, TEXT("Response Received processing img"));
		TArray<uint8> img_data = http_post_req.Get().GetResponse().Get()->GetContent();
		ret = build_image_data(img_data,URL);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Response INVALID!"));
	}
	return ret;
}

EImageFormat UIndexer::get_image_format(FString URL)
{
	EImageFormat fmt = EImageFormat::Invalid;

	if (URL.Contains(".jpg", ESearchCase::IgnoreCase))
		fmt = EImageFormat::JPEG;
	else if (URL.Contains(".png", ESearchCase::IgnoreCase))
		fmt = EImageFormat::PNG;
	else if (URL.Contains(".bmp", ESearchCase::IgnoreCase))
		fmt = EImageFormat::BMP;
	return fmt;
}

UTexture2D* UIndexer::build_image_data(TArray<uint8> img_data,FString URL)
{
	int32 width = 0, height = 0;
	UTexture2D* img = NULL;
	EPixelFormat pxl_format = PF_B8G8R8A8;
	EImageFormat img_format = get_image_format(URL);//get the image format nicely!

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(img_format);
	
	if (ImageWrapper && ImageWrapper.Get()->SetCompressed(img_data.GetData(),img_data.Num()))
	{
		TArray64<uint8>  Uncompressed;
		if (ImageWrapper.Get()->GetRaw(Uncompressed))
		{
			width = ImageWrapper.Get()->GetWidth();
			height = ImageWrapper.Get()->GetHeight();

			img = UTexture2D::CreateTransient(width, height, pxl_format);
			if (!img) return NULL;//nothing to do if it doesn't load!

			void* TextureData = img->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, Uncompressed.GetData(), Uncompressed.Num());
			img->GetPlatformData()->Mips[0].BulkData.Unlock();

			img->UpdateResource();
		}
	}
	return img;
}

TFuture<bool> UIndexer::Ping(int64 chainID)
{
	auto Request = HTTPPost(chainID, "Ping", "");
	TFunction<bool(FString)> MakeResponse = [this](FString Input){return BuildResponse<FPingReturn>(Input).status;}
	return PipeAsync(Request, MakeResponse);
}

TFuture<FVersion> UIndexer::Version(int64 chainID)
{
	auto Request = HTTPPost(chainID, "Version", "");
	TFunction<FVersion(FString)> MakeResponse = [this](FString Input){return BuildResponse<FVersionReturn>(Input).version;};
	return PipeAsync(Request, MakeResponse);
}

TFuture<FRuntimeStatus> UIndexer::RunTimeStatus(int64 chainID)
{
	auto Request = HTTPPost(chainID, "RuntimeStatus", "");
	TFunction<FRuntimeStatus (FString)> MakeResponse = [this](FString Input){return  BuildResponse<FRuntimeStatusReturn>(Input).status;};
	return PipeAsync(Request, MakeResponse);
}

TFuture<int64> UIndexer::GetChainID(int64 chainID)
{
	auto Request = HTTPPost(chainID, "GetChainID", "");
	TFunction<int64 (FString)> MakeResponse = [this](FString Input){return BuildResponse<FGetChainIDReturn>(Input).chainID;};
	return PipeAsync(Request, MakeResponse);
}

void UIndexer::GetEtherBalance(int64 chainID, FString accountAddr)
{//since we are given a raw accountAddress we compose the json arguments here to put in the request manually
	FString json_arg = "{\"accountAddress\":\"";
	json_arg += accountAddr;
	json_arg += "\"}";
	//FGetEtherBalanceReturn response = BuildResponse<FGetEtherBalanceReturn>(HTTPPost(chainID, "GetEtherBalance", json_arg));
//new get eth balance call? PROBLEM since params is a json array and they can contain multiple differing data types unreal won't be able to store it in any other form
// 	   other than json array meaning to get what we need we'd need to re write how the indexer works currently :(
	//Request
//'{"jsonrpc":"2.0","method":"eth_getBalance","params":["0x407d73d8a49eeb85d32cf465507dd71d507100c1", "latest"],"id":1}'

	//Response
//{"id":1 , "jsonrpc" : "2.0" , "result" : "0x0234c8a3397aab58" }

	this->Url(chainID, "GetEtherBalance");
	//this->async_request(this->Url(chainID, "GetEtherBalance"), json_arg, &UIndexer::get_ether_handler);
	FName handler = "get_ether_handler_2";
	this->async_request_test(this->Url(chainID, "GetEtherBalance"), json_arg, 7, &UIndexer::get_ether_handler);
}

//args should be of type FGetTokenBalancesArgs we need to parse these things down to json strings!
FGetTokenBalancesReturn UIndexer::GetTokenBalances(int64 chainID, FGetTokenBalancesArgs args)
{
	return BuildResponse<FGetTokenBalancesReturn>(HTTPPost(chainID, "GetTokenBalances", BuildArgs<FGetTokenBalancesArgs>(args)).Get());
}

FGetTokenSuppliesReturn UIndexer::GetTokenSupplies(int64 chainID, FGetTokenSuppliesArgs args)
{
	return BuildResponse<FGetTokenSuppliesReturn>(HTTPPost(chainID, "GetTokenSupplies", BuildArgs<FGetTokenSuppliesArgs>(args)).Get());
}

FGetTokenSuppliesMapReturn UIndexer::GetTokenSuppliesMap(int64 chainID, FGetTokenSuppliesMapArgs args)
{
	return BuildResponse<FGetTokenSuppliesMapReturn>(HTTPPost(chainID, "GetTokenSuppliesMap", BuildArgs<FGetTokenSuppliesMapArgs>(args)).Get());
}

FGetBalanceUpdatesReturn UIndexer::GetBalanceUpdates(int64 chainID, FGetBalanceUpdatesArgs args)
{
	return BuildResponse<FGetBalanceUpdatesReturn>(HTTPPost(chainID, "GetBalanceUpdates", BuildArgs<FGetBalanceUpdatesArgs>(args)).Get());
}

FGetTransactionHistoryReturn UIndexer::GetTransactionHistory(int64 chainID, FGetTransactionHistoryArgs args)
{
	return BuildResponse<FGetTransactionHistoryReturn>(HTTPPost(chainID, "GetTransactionHistory", BuildArgs<FGetTransactionHistoryArgs>(args)).Get());
}

void UIndexer::async_request(FString url, FString json, void(UIndexer::* handler)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful))
{
	FString response = "[FAILURE]";
	FHttpRequestPtr http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("POST");
	http_post_req->SetHeader("Content-Type", "application/json");
	http_post_req->SetURL(url);
	http_post_req->SetContentAsString(json);
	http_post_req->OnProcessRequestComplete().BindUObject(this, handler);//here we pass in one of our handler functions to do the processing!
	http_post_req->ProcessRequest();
}

void UIndexer::async_request_test(FString url, FString json,int32 i_index, void(UIndexer::* handler)(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful))
{
	FString response = "[FAILURE]";
	FHttpRequestPtr http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("POST");
	http_post_req->SetHeader("Content-Type", "application/json");
	http_post_req->SetURL(url);
	http_post_req->SetContentAsString(json);
	//http_post_req->OnProcessRequestComplete().BindRaw(this,handler);
	//http_post_req->ProcessRequest();
}

void UIndexer::get_ether_handler(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString rep_content = "[error]";
	if (bWasSuccessful)
	{//Parse the json response
		Response = Request.Get()->GetResponse();
		rep_content = Response.Get()->GetContentAsString();//use our rep handler instead!
		FGetEtherBalanceReturn response = BuildResponse<FGetEtherBalanceReturn>(rep_content);//build our response!
		this->bck_mngr->update_ether_balance(response.balance.balanceWei);//let the backend know we are done!
	}
	else
	{
		switch (Request->GetStatus()) {
		case EHttpRequestStatus::Failed_ConnectionError:
			UE_LOG(LogTemp, Error, TEXT("Connection failed."));
		default:
			UE_LOG(LogTemp, Error, TEXT("Request failed."));
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Response: %s"), *rep_content);
}
