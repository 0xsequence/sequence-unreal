// Fill out your copyright notice in the Description page of Project Settings.

#include "ObjectHandler.h"
#include "Sequence_Backend_Manager.h"

void UObjectHandler::setup_string_handler(int32 i_index, string_handler handler_in, UObject * this_ref)
{
	this->insertion_index = i_index;
	this->main_string_handler = handler_in;
	this->main_this_ref = this_ref;
	this->string_handler_ready = true;//ready to be used!
}

void UObjectHandler::setup_raw_handler(int32 i_index, raw_handler handler_in, UObject * this_ref)
{
	this->insertion_index = i_index;
	this->main_raw_handler = handler_in;
	this->main_this_ref = this_ref;
	this->raw_handler_ready = true;//ready to be used!
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
		(this->* (main_raw_handler))(Response.Get()->GetContent(), Request.Get()->GetURL());
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

bool UObjectHandler::request_json(FString URL, FString json_args)
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

bool UObjectHandler::request_raw(FString URL)
{
	UE_LOG(LogTemp, Display , TEXT("Fetching Raw From: %s"),*URL);
	TSharedRef<IHttpRequest> http_post_req = FHttpModule::Get().CreateRequest();
	http_post_req->SetVerb("GET");
	http_post_req->SetURL(URL);
	http_post_req->OnProcessRequestComplete().BindUObject(this,&UObjectHandler::handle_request_raw);
	http_post_req->ProcessRequest();
	return http_post_req.Get().GetStatus() == EHttpRequestStatus::Processing || http_post_req.Get().GetStatus() == EHttpRequestStatus::Succeeded;
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