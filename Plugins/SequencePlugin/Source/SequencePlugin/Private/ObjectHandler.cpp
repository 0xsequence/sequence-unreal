// Copyright 2024 Horizon Blockchain Games Inc. All rights reserved.
#include "ObjectHandler.h"
#include "TextureResource.h"

TMap<FString, UTexture2D*> UObjectHandler::GetProcessedImages()
{
	return this->StoredResponses;
}

void UObjectHandler::OnDone()
{
	if (FOnDoneImageProcessingDelegate.IsBound())
	{
		UE_LOG(LogTemp, Display, TEXT("[Delegate bound calling bound UFUNCTION]"));
		FOnDoneImageProcessingDelegate.ExecuteIfBound();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Delegate Not bound in UObjectHandler]"));
	}
}

void UObjectHandler::Setup(bool RawCacheEnabled)
{
	this->Syncer = NewObject<USyncer>();
	this->Syncer->SetupForTesting("ImageHandler_"+this->GetName());
	//binding will occur here
	//need to bind to OnDone
	this->Syncer->OnDoneDelegate.BindUFunction(this, "OnDone");
	this->UseRawCache = RawCacheEnabled;
}

/*
* Used for using a hardset custom image format for all image requests
* made through this object, if you want to use the computable format use the other setup call
*/
void UObjectHandler::SetupCustomFormat(bool RawCacheEnabled, EImageFormat Format)
{
	this->Syncer = NewObject<USyncer>();
	this->CustomFormat = Format;
	this->bUseCustomFormat = true;
	this->Syncer->OnDoneDelegate.BindUFunction(this, "OnDone");
	this->UseRawCache = RawCacheEnabled;
}

void UObjectHandler::StoreImageData(UTexture2D* Image, FString URL)
{
	this->StoredResponses.Add(TPair<FString,UTexture2D*>(URL,Image));
	UE_LOG(LogTemp, Display, TEXT("[Image stored]"));
	this->Syncer->Decrement();
	//this is when we would consider a response satisfied
}

void UObjectHandler::HandleRequestRaw(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString rep_content = "[error]";
	if (bWasSuccessful)
	{//Parse the json response
		Response = Request.Get()->GetResponse();//allows for more independence!
		//cache our response first!
		TArray<uint8> response_data = Response.Get()->GetContent();
		FString response_url = Request.Get()->GetURL();
		if (this->UseRawCache)
			this->AddToCache(response_url, response_data);//cache it if we need it!

		//build the image data and store it! also decrements our active request count!
		this->StoreImageData(BuildImgData(response_data, response_url), response_url);
	}
	else//error
	{
		UE_LOG(LogTemp, Error, TEXT("Connection Error."));
		//in the event we failed or errored out we should decrement
		this->Syncer->Decrement();
	}
}

TArray<FString> UObjectHandler::FilterURLs(TArray<FString> Urls)
{
	TArray<FString> filteredUrls;

	for (FString url : Urls)
	{//note we don't support .svg or .gif, and we should filter out empty / invalid urls
		bool valid = true;
		valid &= !url.Contains(".gif");
		valid &= !url.Contains(".svg");
		if (valid && url.Len() > 7 && (url.Contains("http://") || url.Contains("https://")))
		{
			filteredUrls.Add(url);
		}
	}

	return filteredUrls;
}

bool UObjectHandler::CheckRawCache(FString URL, TArray<uint8>* RawData)
{
	bool cache_hit = this->Cache.Contains(URL);

	if (cache_hit)
	{
		UE_LOG(LogTemp, Display, TEXT("Raw Cache hit"));
		*RawData = this->Cache.Find(URL)->RawData;//get the data in the cache!
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Raw Cache miss"));
	}
	return cache_hit;
}

void UObjectHandler::AddToCache(FString URL, TArray<uint8> RawData)
{
	if (!this->Cache.Contains(URL) || this->Cache.Num() == 0)
	{
		if (this->CanAddToCache(RawData.Num()))//do we have room in the cache?
		{//we have room in the cache for more!
			FRawData new_cache_entry;
			new_cache_entry.RawData = RawData;
			this->Cache.Add(TTuple<FString, FRawData>(URL, new_cache_entry));//add the new cache entry if we don't already have it!
			this->CurrentCacheSize += RawData.Num();//add the bytes in to keep track!
		}
		else
		{//we now will clear data out at random until we can fit it
			TArray<FString> cache_keys;
			FString r_key;
			FRawData cached_data;

			//I include a check here incase we download an image that is just massive so we don't spin forever!
			while (this->Cache.Num() > 0 && (this->CurrentCacheSize + RawData.Num()) > this->MaxCacheSize)
			{
				this->Cache.GetKeys(cache_keys);//get the keys!
				r_key = cache_keys[FMath::RandRange(0, cache_keys.Num() - 1)];//get a random key!
				cached_data = *this->Cache.Find(r_key);
				this->CurrentCacheSize -= cached_data.RawData.Num();//remove the count!
				this->Cache.Remove(r_key);//remove it from the cache!
			}
			
			//now that we have room for a new entry add it in!
			FRawData new_cache_entry;
			new_cache_entry.RawData = RawData;
			this->Cache.Add(TTuple<FString, FRawData>(URL, new_cache_entry));//add the new cache entry if we don't already have it!
			this->CurrentCacheSize += RawData.Num();//add the bytes in to keep track!
		}
	}
}

//used to clear the contents of the raw cache!
void UObjectHandler::ClearRawCache()
{
	this->CurrentCacheSize = 0;//nothing in here!
	this->Cache.Empty();//clear the cache!
	this->Cache.Shrink();//remove all slack as well
}

bool UObjectHandler::CanAddToCache(int32 ByteCountToAdd)
{
	return (this->CurrentCacheSize + ByteCountToAdd) <= (this->MaxCacheSize);
}

bool UObjectHandler::RequestRawBase(FString URL)
{
	UE_LOG(LogTemp, Display, TEXT("Fetching Raw From: %s"), *URL);

	if (this->UseRawCache)//are we using the cache?
	{
		TArray<uint8> cached_data, * cached_data_ptr;
		cached_data_ptr = &cached_data;
		bool cache_hit = this->CheckRawCache(URL, cached_data_ptr);
		if (cache_hit)
		{
			this->StoreImageData(BuildImgData(cached_data, URL), URL);
			return true; //we are done here!
		}
	}
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("GET");
	http_post_req->SetURL(URL);
	http_post_req->SetTimeout(15);
	http_post_req->OnProcessRequestComplete().BindUObject(this, &UObjectHandler::HandleRequestRaw);
	http_post_req->ProcessRequest();
	return http_post_req.Get().GetStatus() == EHttpRequestStatus::Processing || http_post_req.Get().GetStatus() == EHttpRequestStatus::Succeeded;
}

void UObjectHandler::RequestImage(FString URL)
{
	this->Syncer->Increment();
	this->RequestRawBase(URL);
}

void UObjectHandler::RequestImages(TArray<FString> URLs)
{
	//this will filter out bad urls saving on compute
	TArray<FString> filteredUrls = this->FilterURLs(URLs);
	this->Syncer->Increase(filteredUrls.Num());//inc for all requests
	for (FString url : filteredUrls)
	{
		this->RequestRawBase(url);
	}
}

EImageFormat UObjectHandler::GetImgFormat(FString URL)
{
	EImageFormat fmt = EImageFormat::PNG;//default!

	if (URL.Contains(".jpg", ESearchCase::IgnoreCase))
		fmt = EImageFormat::JPEG;
	else if (URL.Contains(".png", ESearchCase::IgnoreCase))
		fmt = EImageFormat::PNG;
	else if (URL.Contains(".bmp", ESearchCase::IgnoreCase))
		fmt = EImageFormat::BMP;
	else if (URL.Contains(".hdr", ESearchCase::IgnoreCase))
		fmt = EImageFormat::HDR;
	else if (URL.Contains(".tiff", ESearchCase::IgnoreCase) || URL.Contains(".tif", ESearchCase::IgnoreCase))
		fmt = EImageFormat::TIFF;
	else if (URL.Contains(".tga", ESearchCase::IgnoreCase))
		fmt = EImageFormat::TGA;
	return fmt;
}

UTexture2D* UObjectHandler::TryBuildImage(TArray<uint8> ImgData, EImageFormat Format)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(Format);
	UTexture2D* img = nullptr;
	EPixelFormat pxl_format = PF_B8G8R8A8;
	int32 width = 0, height = 0;

	if (ImageWrapper)
	{
		if (ImageWrapper.Get()->SetCompressed(ImgData.GetData(), ImgData.Num()))
		{
			TArray64<uint8>  Uncompressed;
			if (ImageWrapper.Get()->GetRaw(Uncompressed))
			{
				width = ImageWrapper.Get()->GetWidth();
				height = ImageWrapper.Get()->GetHeight();

				img = UTexture2D::CreateTransient(width, height, pxl_format);
				if (!img) return nullptr;//nothing to do if it doesn't load!

				void* TextureData = img->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, Uncompressed.GetData(), Uncompressed.Num());
				img->GetPlatformData()->Mips[0].BulkData.Unlock();

				img->UpdateResource();
			}//valid fetch
		}//valid compression
	}//valid image wrapper

	return img;
}

UTexture2D* UObjectHandler::BuildImgData(TArray<uint8> ImgData,FString URL)
{
	UE_LOG(LogTemp, Display, TEXT("Image size: [%d]"), ImgData.Num());

	int32 width = 0, height = 0;
	UTexture2D* img = nullptr;
	EPixelFormat pxl_format = PF_B8G8R8A8;
	EImageFormat img_format;

	if (this->bUseCustomFormat)
	{
		img_format = this->CustomFormat;
	}
	else
	{
		img_format = GetImgFormat(URL);//get the image format nicely
	}

	img = this->TryBuildImage(ImgData,img_format);

	if (!img)
	{//try again with forced Jpeg our default choice in the event of no .type specified is .png
		img = this->TryBuildImage(ImgData, EImageFormat::JPEG);
	}

	if (!img)
	{//still no dice possible we don't support this in unreal!
		UE_LOG(LogTemp, Warning, TEXT("Bad Image format chosen for: [%s]"), *URL);
	}

	return img;
}