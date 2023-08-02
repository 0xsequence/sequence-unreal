// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectHandler.h"
#include "Sequence_Backend_Manager.h"

void UObjectHandler::setup_string_handler(string_handler handler_in, UObject * this_ref)
{
	this->main_string_handler = handler_in;
	this->main_this_ref = this_ref;
	this->string_handler_ready = true;//ready to be used!
}

void UObjectHandler::setup_raw_handler(raw_handler handler_in, UObject * this_ref,bool raw_cache_enabled)
{
	this->main_raw_handler = handler_in;
	this->main_this_ref = this_ref;
	this->raw_handler_ready = true;//ready to be used!
	this->use_raw_cache = raw_cache_enabled;
}

void UObjectHandler::handle_request_string(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{//Parse the json response
		Response = Request.Get()->GetResponse();
		(this->* (main_string_handler))(Response.Get()->GetContentAsString(), Request.Get()->GetURL());
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
}

void UObjectHandler::handle_request_raw(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString rep_content = "[error]";
	if (bWasSuccessful)
	{//Parse the json response
		Response = Request.Get()->GetResponse();//allows for more independence!
		//cache our response first!
		TArray<uint8> response_data = Response.Get()->GetContent();
		FString response_url = Request.Get()->GetURL();
		if (this->use_raw_cache)
			this->add_to_cache(response_url, response_data);//cache it if we need it!
		(this->* (main_raw_handler))(response_data, response_url);
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
}

bool UObjectHandler::check_raw_cache(FString URL, TArray<uint8>* raw_data)
{
	bool cache_hit = this->cache.Contains(URL);

	if (cache_hit)
	{
		UE_LOG(LogTemp, Display, TEXT("Raw Cache hit"));
		*raw_data = this->cache.Find(URL)->raw_data;//get the data in the cache!
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Raw Cache miss"));
	}

	return cache_hit;
}

void UObjectHandler::add_to_cache(FString URL, TArray<uint8> raw_data)
{
	if (!this->cache.Contains(URL) || this->cache.Num() == 0)
	{
		if (this->can_add_to_cache(raw_data.Num()))//do we have room in the cache?
		{//we have room in the cache for more!
			FRawData new_cache_entry;
			new_cache_entry.raw_data = raw_data;
			this->cache.Add(TTuple<FString, FRawData>(URL, new_cache_entry));//add the new cache entry if we don't already have it!
			this->current_cache_size += raw_data.Num();//add the bytes in to keep track!
		}
		else
		{//we now will clear data out at random until we can fit it
			TArray<FString> cache_keys;
			FString r_key;
			FRawData cached_data;

			//I include a check here incase we download an image that is just massive so we don't spin forever!
			while (this->cache.Num() > 0 && (this->current_cache_size + raw_data.Num()) > this->max_cache_size)
			{
				this->cache.GetKeys(cache_keys);//get the keys!
				r_key = cache_keys[FMath::RandRange(0, cache_keys.Num() - 1)];//get a random key!
				cached_data = *this->cache.Find(r_key);
				this->current_cache_size -= cached_data.raw_data.Num();//remove the count!
				this->cache.Remove(r_key);//remove it from the cache!
			}
			
			//now that we have room for a new entry add it in!
			FRawData new_cache_entry;
			new_cache_entry.raw_data = raw_data;
			this->cache.Add(TTuple<FString, FRawData>(URL, new_cache_entry));//add the new cache entry if we don't already have it!
			this->current_cache_size += raw_data.Num();//add the bytes in to keep track!
		}
	}
}


//used to clear the contents of the raw cache!
void UObjectHandler::clear_raw_cache()
{
	this->current_cache_size = 0;//nothing in here!
	this->cache.Empty();//clear the cache!
	this->cache.Shrink();//remove all slack as well
}


bool UObjectHandler::can_add_to_cache(int32 byte_count_to_add)
{
	return (this->current_cache_size + byte_count_to_add) <= (this->max_cache_size);
}


void UObjectHandler::add_to_insertion_indices(FString URL, int32 i_index)
{
	if (i_index >= 0)
	{
		this->insertion_indices.Add(MakeTuple(URL,i_index));
	}
}

void UObjectHandler::remove_from_insertion_indices(FString URL)
{
	this->insertion_indices.Remove(URL);//if this key is present get rid of it!
}

bool UObjectHandler::request_json_base(FString URL, FString json_args)
{
	UE_LOG(LogTemp, Display, TEXT("Fetching JSON From: %s"), *URL);
	FString response = "[NO RESPONSE]";
	//Now we create the post request
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("POST");
	http_post_req->SetHeader("Content-Type", "application/json");
	http_post_req->SetHeader("Accept", "application/json");
	http_post_req->SetURL(URL);
	http_post_req->SetContentAsString(json_args);//args will need to be a json object converted to a string
	http_post_req->OnProcessRequestComplete().BindUObject(this, &UObjectHandler::handle_request_string);
	http_post_req->ProcessRequest();
	return http_post_req.Get().GetStatus() == EHttpRequestStatus::Processing || http_post_req.Get().GetStatus() == EHttpRequestStatus::Succeeded;
}

bool UObjectHandler::request_raw_base(FString URL)
{
	UE_LOG(LogTemp, Display, TEXT("Fetching Raw From: %s"), *URL);

	if (this->use_raw_cache)//are we using the cache?
	{
		TArray<uint8> cached_data, * cached_data_ptr;
		cached_data_ptr = &cached_data;
		bool cache_hit = this->check_raw_cache(URL, cached_data_ptr);
		if (cache_hit)
		{
			(this->* (main_raw_handler))(cached_data, URL);//call our raw handler with the cached data directly
			return true; //we are done here!
		}
	}
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("GET");
	http_post_req->SetURL(URL);
	http_post_req->OnProcessRequestComplete().BindUObject(this, &UObjectHandler::handle_request_raw);
	http_post_req->ProcessRequest();
	return http_post_req.Get().GetStatus() == EHttpRequestStatus::Processing || http_post_req.Get().GetStatus() == EHttpRequestStatus::Succeeded;
}

bool UObjectHandler::request_json(FString URL, FString json_args)
{
	return request_json_base(URL, json_args);
}

bool UObjectHandler::request_json_indexed(FString URL, FString json_args, int32 index)
{
	this->add_to_insertion_indices(URL, index);
	return this->request_json_base(URL, json_args);
}

bool UObjectHandler::request_raw(FString URL)
{
	return this->request_raw_base(URL);
}

bool UObjectHandler::request_raw_indexed(FString URL, int32 index)
{
	this->add_to_insertion_indices(URL, index);
	return this->request_raw(URL);
}

void UObjectHandler::img_handler(TArray<uint8> response,FString URL)
{
	ASequence_Backend_Manager* bknd = Cast<ASequence_Backend_Manager, UObject>(TObjectPtr<UObject>(this->main_this_ref));
	if (!bknd)
		return;//nothing more to do if this isn't valid
	bknd->add_img(build_img_data(response,URL));
}

EImageFormat UObjectHandler::get_img_format(FString URL)
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

UTexture2D* UObjectHandler::build_img_data(TArray<uint8> img_data,FString URL)
{
	int32 width = 0, height = 0;
	UTexture2D* img = NULL;
	EPixelFormat pxl_format = PF_B8G8R8A8;
	EImageFormat img_format = get_img_format(URL);//get the image format nicely!

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(img_format);

	if (ImageWrapper && ImageWrapper.Get()->SetCompressed(img_data.GetData(), img_data.Num()))
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